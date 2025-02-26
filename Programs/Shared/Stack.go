// Copyright mzoesch. All rights reserved.

package Shared

type Stack[T any] struct {
    Data []T
}

func NewStack[T any]() Stack[T] {
    return Stack[T]{}
}

func (s *Stack[T]) Push(v T) {
    s.Data = append(s.Data, v)
}

func (s *Stack[T]) Pop() (T, bool) {
    if len(s.Data) == 0 {
        var t T
        return t, false
    }
    s.Data = s.Data[:len(s.Data)-1]
    if len(s.Data) == 0 {
        var t T
        return t, false
    }
    return s.Data[len(s.Data)-1], true
}

func (s *Stack[T]) Peek() (T, bool) {
    if len(s.Data) == 0 {
        var t T
        return t, false
    }
    return s.Data[len(s.Data)-1], true
}

func (s *Stack[T]) IsEmpty() bool {
    return len(s.Data) == 0
}

func (s *Stack[T]) Empty() {
    for _, _ = range s.Data {
        _, b := s.Pop()
        if !b {
            panic("Stack is empty.")
        }
        continue
    }
    if !s.IsEmpty() {
        panic("Stack is not empty.")
    }
    return
}
