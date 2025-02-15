/*
 * Copyright (c) 2021, Andreas Kling <kling@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Forward.h>
#include <AK/Span.h>
#include <LibJS/Bytecode/Executable.h>
#include <LibJS/Forward.h>
#include <LibJS/SourceRange.h>

#define ENUMERATE_BYTECODE_OPS(O)      \
    O(Add)                             \
    O(Append)                          \
    O(AsyncIteratorClose)              \
    O(Await)                           \
    O(BitwiseAnd)                      \
    O(BitwiseNot)                      \
    O(BitwiseOr)                       \
    O(BitwiseXor)                      \
    O(BlockDeclarationInstantiation)   \
    O(Call)                            \
    O(CallWithArgumentArray)           \
    O(ConcatString)                    \
    O(ContinuePendingUnwind)           \
    O(CopyObjectExcludingProperties)   \
    O(CreateLexicalEnvironment)        \
    O(CreateVariable)                  \
    O(Decrement)                       \
    O(DeleteById)                      \
    O(DeleteByIdWithThis)              \
    O(DeleteByValue)                   \
    O(DeleteByValueWithThis)           \
    O(DeleteVariable)                  \
    O(Div)                             \
    O(EnterUnwindContext)              \
    O(EnterObjectEnvironment)          \
    O(Exp)                             \
    O(GetById)                         \
    O(GetByIdWithThis)                 \
    O(GetByValue)                      \
    O(GetByValueWithThis)              \
    O(GetCalleeAndThisFromEnvironment) \
    O(GetIterator)                     \
    O(GetMethod)                       \
    O(GetNewTarget)                    \
    O(GetImportMeta)                   \
    O(GetObjectPropertyIterator)       \
    O(GetPrivateById)                  \
    O(GetVariable)                     \
    O(GetGlobal)                       \
    O(GetLocal)                        \
    O(GreaterThan)                     \
    O(GreaterThanEquals)               \
    O(HasPrivateId)                    \
    O(ImportCall)                      \
    O(In)                              \
    O(Increment)                       \
    O(InstanceOf)                      \
    O(IteratorClose)                   \
    O(IteratorNext)                    \
    O(IteratorResultDone)              \
    O(IteratorResultValue)             \
    O(IteratorToArray)                 \
    O(Jump)                            \
    O(JumpConditional)                 \
    O(JumpNullish)                     \
    O(JumpUndefined)                   \
    O(LeaveLexicalEnvironment)         \
    O(LeaveUnwindContext)              \
    O(LeftShift)                       \
    O(LessThan)                        \
    O(LessThanEquals)                  \
    O(Load)                            \
    O(LoadImmediate)                   \
    O(LooselyEquals)                   \
    O(LooselyInequals)                 \
    O(Mod)                             \
    O(Mul)                             \
    O(NewArray)                        \
    O(NewBigInt)                       \
    O(NewClass)                        \
    O(NewFunction)                     \
    O(NewObject)                       \
    O(NewRegExp)                       \
    O(NewString)                       \
    O(NewTypeError)                    \
    O(Not)                             \
    O(PushDeclarativeEnvironment)      \
    O(PutById)                         \
    O(PutByIdWithThis)                 \
    O(PutByValue)                      \
    O(PutByValueWithThis)              \
    O(PutPrivateById)                  \
    O(ResolveThisBinding)              \
    O(ResolveSuperBase)                \
    O(Return)                          \
    O(RightShift)                      \
    O(ScheduleJump)                    \
    O(SetVariable)                     \
    O(SetLocal)                        \
    O(Store)                           \
    O(StrictlyEquals)                  \
    O(StrictlyInequals)                \
    O(Sub)                             \
    O(SuperCallWithArgumentArray)      \
    O(Throw)                           \
    O(ThrowIfNotObject)                \
    O(ThrowIfNullish)                  \
    O(ToNumeric)                       \
    O(Typeof)                          \
    O(TypeofVariable)                  \
    O(TypeofLocal)                     \
    O(UnaryMinus)                      \
    O(UnaryPlus)                       \
    O(UnsignedRightShift)              \
    O(Yield)

namespace JS::Bytecode {

class alignas(void*) Instruction {
public:
    constexpr static bool IsTerminator = false;

    enum class Type {
#define __BYTECODE_OP(op) \
    op,
        ENUMERATE_BYTECODE_OPS(__BYTECODE_OP)
#undef __BYTECODE_OP
    };

    Type type() const { return m_type; }
    size_t length() const { return m_length; }
    DeprecatedString to_deprecated_string(Bytecode::Executable const&) const;
    ThrowCompletionOr<void> execute(Bytecode::Interpreter&) const;
    static void destroy(Instruction&);

    // FIXME: Find a better way to organize this information
    void set_source_record(SourceRecord rec) { m_source_record = rec; }
    SourceRecord source_record() const { return m_source_record; }

protected:
    Instruction(Type type, size_t length)
        : m_type(type)
        , m_length(length)
    {
    }

private:
    SourceRecord m_source_record {};
    Type m_type {};
    size_t m_length {};
};

class InstructionStreamIterator {
public:
    InstructionStreamIterator(ReadonlyBytes bytes, Executable const* executable = nullptr)
        : m_begin(bytes.data())
        , m_end(bytes.data() + bytes.size())
        , m_ptr(bytes.data())
        , m_executable(executable)
    {
    }

    size_t offset() const { return m_ptr - m_begin; }
    bool at_end() const { return m_ptr >= m_end; }

    Instruction const& operator*() const { return dereference(); }

    ALWAYS_INLINE void operator++()
    {
        m_ptr += dereference().length();
    }

    UnrealizedSourceRange source_range() const;
    RefPtr<SourceCode> source_code() const;

private:
    Instruction const& dereference() const { return *reinterpret_cast<Instruction const*>(m_ptr); }

    u8 const* m_begin { nullptr };
    u8 const* m_end { nullptr };
    u8 const* m_ptr { nullptr };
    Executable const* m_executable { nullptr };
};

}
