#pragma once
#include "UBF.h"
#include "ubf_interpreter.h"
#include "UBFLog.h"

#define IMPLEMENT_MANAGED_TYPE_CONSTRUCTOR(StructName, Type)                     \
public:                                                                          \
explicit StructName(Type* InRustPtr) : RustPtr(InRustPtr)		    	         \
{                                                                                \
                                                                                 \
}                                                                                \
explicit StructName() : RustPtr(nullptr)		    	                         \
{                                                                                \
                                                                                 \
}                                                                                \

#define IMPLEMENT_MANAGED_TYPE(StructName, Type, ReleaseFunc, RetainFunc)        \
protected:                                                                       \
    Type* RustPtr;                                                               \
                                                                                 \
public:                                                                          \
    ~StructName()																 \
    {                                                                            \
        if (RustPtr)                                                             \
        {                                                                        \
            CALL_RUST_FUNC(ReleaseFunc)(RustPtr);                                \
            RustPtr = nullptr;                                                   \
        }                                                                        \
    }                                                                            \
                                                                                 \
                                                                                 \
    StructName(const StructName& Other) : RustPtr(Other.RustPtr)				 \
    {                                                                            \
        if (RustPtr)                                                             \
        {                                                                        \
            RustPtr = CALL_RUST_FUNC(RetainFunc)(RustPtr);                       \
        }                                                                        \
    }                                                                            \
                                                                                 \
    StructName& operator=(const StructName& Other)								 \
    {                                                                            \
        if (RustPtr != Other.RustPtr)                                            \
        {                                                                        \
            if (RustPtr)                                                         \
            {                                                                    \
                CALL_RUST_FUNC(ReleaseFunc)(RustPtr);                            \
            }                                                                    \
                                                                                 \
            RustPtr = Other.RustPtr;                                             \
                                                                                 \
            if (RustPtr)                                                         \
            {                                                                    \
                RustPtr = CALL_RUST_FUNC(RetainFunc)(RustPtr);                   \
            }                                                                    \
        }                                                                        \
        return *this;                                                            \
    }                                                                            \
                                                                                 \
    StructName(StructName&& Other) noexcept : RustPtr(Other.RustPtr)			 \
    {                                                                            \
        Other.RustPtr = nullptr;                                                 \
    }                                                                            \
                                                                                 \
    StructName& operator=(StructName&& Other) noexcept							 \
    {                                                                            \
        if (RustPtr != Other.RustPtr)                                            \
        {                                                                        \
            if (RustPtr)                                                         \
            {                                                                    \
                CALL_RUST_FUNC(ReleaseFunc)(RustPtr);                            \
            }                                                                    \
            RustPtr = Other.RustPtr;                                             \
            Other.RustPtr = nullptr;                                             \
        }                                                                        \
        return *this;                                                            \
    }                                                                            \
                                                                                 \
    Type* GetRustPtr() const                                                     \
    {                                                                            \
        return RustPtr;                                                          \
    }
