#pragma once

#include "GCPin.h"
#include "Managed.h"
#include "UBF.h"
#include "UBFLog.h"
#include "UBF/Lib/ubf_interpreter.h"

namespace UBF
{
	struct FDynamicHandleBase
	{
		IMPLEMENT_MANAGED_TYPE_CONSTRUCTOR(FDynamicHandleBase, FFI::Dynamic)
		IMPLEMENT_MANAGED_TYPE(FDynamicHandleBase, FFI::Dynamic, dynamic_release, dynamic_retain)
	};

	/**
	 * Handle for Dynamic rust values.
	 * Use dedicated functions String(), Int(), Float(), Bool(), Foreign(), Array(), Dictionary() to create new DynamicValues
	 * Use Foreign() for any UObject, this will hold on reference to the UObject to prevent GC
	 *
	 * Handle can be copied, when the last handle goes out of scope and rust no longer references the value, it will be deleted
	 */
	struct UBF_API FDynamicHandle final : public FDynamicHandleBase
	{
		FFI::DynamicType Type;
		int AsInt;
		float AsFloat;
		intptr_t AsForeignPtr;
		FString AsString;

	public:
		explicit FDynamicHandle(FFI::Dynamic* RustPtr)
			: FDynamicHandleBase(RustPtr)
		{
			CALL_RUST_FUNC(dynamic_extract)(RustPtr, &Type, &AsInt, &AsFloat, &AsForeignPtr);
		}

		FDynamicHandle(): FDynamicHandleBase(nullptr), Type(), AsInt(0), AsFloat(0), AsForeignPtr(0)
		{
		}

		static FDynamicHandle String(const FString& Value)
		{
			return FDynamicHandle(
				CALL_RUST_FUNC(dynamic_new_string)(TCHAR_TO_UTF16(*Value), Value.Len())
			);
		}
		
		static FDynamicHandle Int(const int32_t Value)
		{
			return FDynamicHandle(
				CALL_RUST_FUNC(dynamic_new_primitive)(FFI::DynamicType::Int, Value, 0)
			);
		}

		static FDynamicHandle Float(const float Value)
		{
			return FDynamicHandle(
				CALL_RUST_FUNC(dynamic_new_primitive)(FFI::DynamicType::Float, 0, Value)
			);
		}

		static FDynamicHandle Bool(const bool Value)
		{
			return FDynamicHandle(
				CALL_RUST_FUNC(dynamic_new_primitive)(FFI::DynamicType::Bool, Value ? 1 : 0, 0)
			);
		}
		
		static FDynamicHandle Foreign(UObject* Object);

		static FDynamicHandle Foreign(void* Data, void (*FreeCallback)(intptr_t))
		{
			return FDynamicHandle(
				CALL_RUST_FUNC(dynamic_new_foreign)(reinterpret_cast<intptr_t>(Data), FreeCallback)
			);
		}

		// Supports raw unhandled PTRs, will call delete once finished
		template <class T>
		static FDynamicHandle ForeignHandled(T* Data)
		{
			return Foreign((void*)Data, [](const intptr_t DataPtr)
			{
				UE_LOG(LogUBF, VeryVerbose, TEXT("FREE DataPtr %lld"), DataPtr);
				const T* Data2 = static_cast<T*>((void*)DataPtr);
				delete Data2;
			});
		}

		static FDynamicHandle Array()
		{
			return FDynamicHandle
			(
				CALL_RUST_FUNC(dynamic_new_array)()
			);
		}

		static FDynamicHandle Dictionary()
		{
			return FDynamicHandle
			(
				CALL_RUST_FUNC(dynamic_new_dictionary)()
			);
		}

		static FDynamicHandle Null();

		bool Push(const FDynamicHandle& Value) const
		{
			return CALL_RUST_FUNC(dynamic_array_push)(RustPtr, Value.RustPtr);
		}

		template <typename T>
		bool TryInterpretAs(T*& Out) const
		{
			if (Type != FFI::DynamicType::Foreign) return false;
			Out = static_cast<T*>((void*)AsForeignPtr);
			return true;
		}

		bool TryInterpretAs(UObject*& Out) const
		{
			if (Type != FFI::DynamicType::Foreign) return false;
			const UGCPin* GCPin = static_cast<UGCPin*>((void*)AsForeignPtr);
			Out = GCPin->Object;
			return true;
		}

		bool TryInterpretAs(FString& Out) const
		{
			Out = nullptr;
			if (Type != FFI::DynamicType::String) return false;

			void* OutStr;
			const uint16_t* OutPtr;
			uintptr_t OutLen;
			
			if (CALL_RUST_FUNC(dynamic_as_string)(RustPtr, &OutStr, &OutPtr, &OutLen))
			{
#if ENGINE_MAJOR_VERSION==5 && ENGINE_MINOR_VERSION > 3
				Out = OutLen == 0 ? FString() : FString::ConstructFromPtrSize(OutPtr, OutLen);
#else
				Out = OutLen == 0 ? FString() : FString(OutLen, OutPtr);
#endif
				CALL_RUST_FUNC(box_release)(OutStr);
				return true;
			}

			UE_LOG(LogUBF, Error, TEXT("Dynamic is not a String"));

			return false;
		}

		bool TryInterpretAs(bool& Out) const
		{
			Out = AsInt != 0;
			return Type == FFI::DynamicType::Bool;
		}

		bool TryInterpretAs(int& Out) const
		{
			Out = AsInt;
			return Type == FFI::DynamicType::Int;
		}

		bool TryInterpretAs(float& Out) const
		{
			Out = AsFloat;
			return Type == FFI::DynamicType::Float;
		}

		template <typename T>
		bool TryInterpretAsArray(TArray<T>& Out) const
		{
			TArray<T>* Context = new TArray<T>();
		
			auto ArrayItr = [](intptr_t Context, FFI::Dynamic* Dynamic) -> bool
			{
				TArray<T>* Array = reinterpret_cast<TArray<T>*>(Context);
				
				FDynamicHandle DynamicHandle(Dynamic);

				T Value;
				if (DynamicHandle.TryInterpretAs(Value))
				{
					Array->Add(Value);
					// UE_LOG(LogUBF, Verbose, TEXT("Interpting value in array {%s}"), *DynamicHandle.ToString());
				}
				else
				{
					UE_LOG(LogUBF, Warning, TEXT("Failed to interpt dynamic in array {%s}"), *DynamicHandle.ToString());
				}
			
				return true;
			};
		
			CALL_RUST_FUNC(dynamic_array_iter)(
				RustPtr,
				reinterpret_cast<intptr_t>(Context),
				ArrayItr
			);

			Out = *Context;
			delete Context;
			return true;
		}

		bool TrySet(const FString& Key, const FDynamicHandle& Value) const
		{
			return CALL_RUST_FUNC(dynamic_dictionary_set)(
					RustPtr,
					TCHAR_TO_UTF16(*Key),
					Key.Len(),
					Value.RustPtr
				);
		}

		static bool IsNull(const FDynamicHandle& Other)
		{
			return Other.IsNull();
		}
		
		bool IsNull() const
		{
			return RustPtr == nullptr;
		}

		FString ToString() const
		{
			switch (Type)
			{
			case FFI::DynamicType::String:
				{
					FString StringValue;
					if (TryInterpretAs(StringValue))
					{
						return StringValue;
					}
					break;
				}

			case FFI::DynamicType::Int:
				{
					int IntValue;
					if (TryInterpretAs(IntValue))
					{
						return FString::FromInt(IntValue);
					}
					break;
				}

			case FFI::DynamicType::Float:
				{
					float FloatValue;
					if (TryInterpretAs(FloatValue))
					{
						return FString::SanitizeFloat(FloatValue);
					}
					break;
				}

			case FFI::DynamicType::Bool:
				{
					bool BoolValue;
					if (TryInterpretAs(BoolValue))
					{
						return BoolValue ? TEXT("True") : TEXT("False");
					}
					break;
				}

			case FFI::DynamicType::Foreign:
				{
					return TEXT("Foreign Pointer");
				}

				// Add handling for other types if necessary
			case FFI::DynamicType::Array:
				{
					return TEXT("Array");
				}

			case FFI::DynamicType::Dictionary:
				{
					return TEXT("Dictionary");
				}

			default:
				return TEXT("Unknown");
			}

			return TEXT("Invalid");
		}


	private:
		static void OnComplete(intptr_t Pointer);
	};
}
