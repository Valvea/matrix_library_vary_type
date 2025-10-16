#!/usr/bin/env python3
"""
Полный пример использования C-библиотеки матриц из Python через ctypes.

Перед запуском убедитесь, что собраны .so-файлы:
    make shared

Запускайте из каталога src (чтобы LD_LIBRARY_PATH указывал на текущую папку):
    LD_LIBRARY_PATH=. python3 python_example.py
"""

from __future__ import annotations

import contextlib
import ctypes
import math
import os
import sys
from pathlib import Path
from typing import Callable, Iterable, List, Sequence


# ── Загрузка библиотек ────────────────────────────────────────────────────────────
LIB_DIR = Path(__file__).resolve().parent
core = ctypes.CDLL(str(LIB_DIR / "libmatrix_core.so"), mode=ctypes.RTLD_GLOBAL)
lib_d = ctypes.CDLL(str(LIB_DIR / "libmatrix_double.so"), mode=ctypes.RTLD_GLOBAL)
lib_i = ctypes.CDLL(str(LIB_DIR / "libmatrix_int.so"), mode=ctypes.RTLD_GLOBAL)
libc = ctypes.CDLL(None)  # нужен для free/fflush


# ── Общие enum-ы ──────────────────────────────────────────────────────────────────
class MatType:
    MAT_INVALID = 0
    MAT_INT32 = 1
    MAT_F64 = 2


class SearchElement:
    FIRST_MIN = 0
    FIRST_MAX = 1
    FIRST_EQUAL = 2
    MODULE_MAX = 3


# ── Структура Matrix (ядро) ───────────────────────────────────────────────────────
class Matrix(ctypes.Structure):
    class RowUnion(ctypes.Union):
        _fields_ = [
            ("d", ctypes.POINTER(ctypes.POINTER(ctypes.c_double))),
            ("i", ctypes.POINTER(ctypes.POINTER(ctypes.c_int))),
            ("vp", ctypes.POINTER(ctypes.c_void_p)),
        ]

    _fields_ = [
        ("rows", ctypes.c_int),
        ("cols", ctypes.c_int),
        ("owns", ctypes.c_int),
        ("type", ctypes.c_int),
        ("block", ctypes.c_void_p),
        ("flat_data", ctypes.c_void_p),
        ("row", RowUnion),
    ]


MatrixPtr = ctypes.POINTER(Matrix)

# double указатели
DoubleRowPtr = ctypes.POINTER(ctypes.POINTER(ctypes.c_double))  # double**
DoubleFlatPtr = ctypes.POINTER(ctypes.c_double)                 # double*

# int указатели
IntRowPtr = ctypes.POINTER(ctypes.POINTER(ctypes.c_int))        # int**
IntFlatPtr = ctypes.POINTER(ctypes.c_int)                       # int*


# ── Прототипы функций ядра ───────────────────────────────────────────────────────
core.mat_new.argtypes = (ctypes.c_int, ctypes.c_int, ctypes.c_int)
core.mat_new.restype = MatrixPtr

core.mat_free.argtypes = (MatrixPtr,)
core.mat_free.restype = None

core.mat_rows_f64.argtypes = (MatrixPtr,)
core.mat_rows_f64.restype = DoubleRowPtr

core.mat_rows_i32.argtypes = (MatrixPtr,)
core.mat_rows_i32.restype = IntRowPtr

core.mat_data_f64.argtypes = (MatrixPtr,)
core.mat_data_f64.restype = DoubleFlatPtr

core.mat_data_i32.argtypes = (MatrixPtr,)
core.mat_data_i32.restype = IntFlatPtr

# ── Прототипы double API ─────────────────────────────────────────────────────────
lib_d.scan_matrix_double.argtypes = (
    DoubleRowPtr,
    ctypes.POINTER(ctypes.c_int),
    ctypes.POINTER(ctypes.c_int),
)
lib_d.scan_matrix_double.restype = DoubleRowPtr

lib_d.create_matrix_double.argtypes = (ctypes.c_int, ctypes.c_int)
lib_d.create_matrix_double.restype = DoubleRowPtr

lib_d.unit_matrix_double.argtypes = (ctypes.c_int, ctypes.c_int)
lib_d.unit_matrix_double.restype = DoubleRowPtr

lib_d.copy_matrix_double.argtypes = (DoubleRowPtr, ctypes.c_int, ctypes.c_int)
lib_d.copy_matrix_double.restype = DoubleRowPtr

lib_d.fill_matrix_double.argtypes = (DoubleRowPtr, ctypes.c_int, ctypes.c_int)
lib_d.fill_matrix_double.restype = ctypes.c_int

lib_d.print_matrix_double.argtypes = (DoubleRowPtr, ctypes.c_int, ctypes.c_int)
lib_d.print_matrix_double.restype = None

lib_d.sort_matrix_double.argtypes = (DoubleRowPtr, ctypes.c_int, ctypes.c_int)
lib_d.sort_matrix_double.restype = DoubleRowPtr

lib_d.free_matrix_double.argtypes = (DoubleRowPtr,)
lib_d.free_matrix_double.restype = None

lib_d.search_array_index_double.argtypes = (
    ctypes.c_int,
    DoubleFlatPtr,
    ctypes.c_double,
    ctypes.c_int,
)
lib_d.search_array_index_double.restype = ctypes.c_int

lib_d.scale_array_double.argtypes = (DoubleFlatPtr, ctypes.c_double, ctypes.c_int)
lib_d.scale_array_double.restype = DoubleFlatPtr

lib_d.scale_array_inplace_double.argtypes = (
    DoubleFlatPtr,
    ctypes.c_double,
    ctypes.c_int,
)
lib_d.scale_array_inplace_double.restype = DoubleFlatPtr

lib_d.add_array_double.argtypes = (DoubleFlatPtr, DoubleFlatPtr, ctypes.c_int)
lib_d.add_array_double.restype = DoubleFlatPtr

lib_d.add_array_inplace_double.argtypes = (
    DoubleFlatPtr,
    DoubleFlatPtr,
    ctypes.c_int,
)
lib_d.add_array_inplace_double.restype = DoubleFlatPtr

lib_d.sub_array_double.argtypes = (DoubleFlatPtr, DoubleFlatPtr, ctypes.c_int)
lib_d.sub_array_double.restype = DoubleFlatPtr

lib_d.sub_array_inplace_double.argtypes = (
    DoubleFlatPtr,
    DoubleFlatPtr,
    ctypes.c_int,
)
lib_d.sub_array_inplace_double.restype = DoubleFlatPtr

lib_d.sum_array_double.argtypes = (DoubleFlatPtr, ctypes.c_int)
lib_d.sum_array_double.restype = ctypes.c_double

lib_d.min_array_double.argtypes = (DoubleFlatPtr, ctypes.c_int)
lib_d.min_array_double.restype = ctypes.c_double

lib_d.max_array_double.argtypes = (DoubleFlatPtr, ctypes.c_int)
lib_d.max_array_double.restype = ctypes.c_double

lib_d.dot_arrays_double.argtypes = (DoubleFlatPtr, DoubleFlatPtr, ctypes.c_int)
lib_d.dot_arrays_double.restype = ctypes.c_double

lib_d.dot_arrays_Hadamard_double.argtypes = (
    DoubleFlatPtr,
    DoubleFlatPtr,
    ctypes.c_int,
)
lib_d.dot_arrays_Hadamard_double.restype = DoubleFlatPtr

lib_d.from_array_to_matrix_double.argtypes = (
    DoubleFlatPtr,
    ctypes.c_int,
    ctypes.c_int,
    ctypes.c_int,
)
lib_d.from_array_to_matrix_double.restype = DoubleRowPtr

lib_d.row_to_array_double.argtypes = (
    DoubleRowPtr,
    DoubleFlatPtr,
    ctypes.c_int,
    ctypes.c_int,
)
lib_d.row_to_array_double.restype = DoubleFlatPtr

lib_d.col_to_array_double.argtypes = (
    DoubleRowPtr,
    DoubleFlatPtr,
    ctypes.c_int,
    ctypes.c_int,
)
lib_d.col_to_array_double.restype = DoubleFlatPtr

lib_d.set_row_double.argtypes = (
    DoubleRowPtr,
    DoubleFlatPtr,
    ctypes.c_int,
    ctypes.c_int,
    ctypes.c_int,
)
lib_d.set_row_double.restype = ctypes.c_int

lib_d.set_col_double.argtypes = (
    DoubleRowPtr,
    DoubleFlatPtr,
    ctypes.c_int,
    ctypes.c_int,
    ctypes.c_int,
)
lib_d.set_col_double.restype = ctypes.c_int

lib_d.swap_rows_double.argtypes = (
    DoubleRowPtr,
    ctypes.c_int,
    ctypes.c_int,
    ctypes.c_int,
)
lib_d.swap_rows_double.restype = None

lib_d.T_matrix_double.argtypes = (DoubleRowPtr, ctypes.c_int, ctypes.c_int)
lib_d.T_matrix_double.restype = DoubleRowPtr

lib_d.T_matrix_inplace_double.argtypes = (
    DoubleRowPtr,
    ctypes.c_int,
    ctypes.c_int,
)
lib_d.T_matrix_inplace_double.restype = DoubleRowPtr

lib_d.dot_matrix_double.argtypes = (
    DoubleRowPtr,
    DoubleRowPtr,
    ctypes.c_int,
    ctypes.c_int,
    ctypes.c_int,
    ctypes.c_int,
)
lib_d.dot_matrix_double.restype = DoubleRowPtr

lib_d.sum_matrix_double.argtypes = (
    DoubleRowPtr,
    DoubleRowPtr,
    ctypes.c_int,
    ctypes.c_int,
)
lib_d.sum_matrix_double.restype = DoubleRowPtr

lib_d.sub_matrix_double.argtypes = (
    DoubleRowPtr,
    DoubleRowPtr,
    ctypes.c_int,
    ctypes.c_int,
)
lib_d.sub_matrix_double.restype = DoubleRowPtr

lib_d.dot_matrix_Hadamard_double.argtypes = (
    DoubleRowPtr,
    DoubleRowPtr,
    ctypes.c_int,
    ctypes.c_int,
)
lib_d.dot_matrix_Hadamard_double.restype = DoubleRowPtr

lib_d.matrix_to_array_double.argtypes = (DoubleRowPtr, ctypes.c_int)
lib_d.matrix_to_array_double.restype = DoubleFlatPtr


class ResultSolve(ctypes.Structure):
    _fields_ = [
        ("status", ctypes.c_int),
        ("determinant", ctypes.c_double),
        ("invers_matrix", DoubleRowPtr),
        ("swaps", ctypes.c_int),
    ]


lib_d.matrix_det_lu_double.argtypes = (
    ctypes.c_int,
    DoubleRowPtr,
    ctypes.c_int,
    ctypes.c_int,
)
lib_d.matrix_det_lu_double.restype = ResultSolve


# ── Прототипы int API ─────────────────────────────────────────────────────────────
lib_i.scan_matrix_int.argtypes = (
    IntRowPtr,
    ctypes.POINTER(ctypes.c_int),
    ctypes.POINTER(ctypes.c_int),
)
lib_i.scan_matrix_int.restype = IntRowPtr

lib_i.create_matrix_int.argtypes = (ctypes.c_int, ctypes.c_int)
lib_i.create_matrix_int.restype = IntRowPtr

lib_i.unit_matrix_int.argtypes = (ctypes.c_int, ctypes.c_int)
lib_i.unit_matrix_int.restype = IntRowPtr

lib_i.copy_matrix_int.argtypes = (IntRowPtr, ctypes.c_int, ctypes.c_int)
lib_i.copy_matrix_int.restype = IntRowPtr

lib_i.fill_matrix_int.argtypes = (IntRowPtr, ctypes.c_int, ctypes.c_int)
lib_i.fill_matrix_int.restype = ctypes.c_int

lib_i.print_matrix_int.argtypes = (IntRowPtr, ctypes.c_int, ctypes.c_int)
lib_i.print_matrix_int.restype = None

lib_i.sort_matrix_int.argtypes = (IntRowPtr, ctypes.c_int, ctypes.c_int)
lib_i.sort_matrix_int.restype = IntRowPtr

lib_i.free_matrix_int.argtypes = (IntRowPtr,)
lib_i.free_matrix_int.restype = None

lib_i.search_array_index_int.argtypes = (
    ctypes.c_int,
    IntFlatPtr,
    ctypes.c_int,
    ctypes.c_int,
)
lib_i.search_array_index_int.restype = ctypes.c_int

lib_i.scale_array_int.argtypes = (IntFlatPtr, ctypes.c_int, ctypes.c_int)
lib_i.scale_array_int.restype = IntFlatPtr

lib_i.scale_array_inplace_int.argtypes = (
    IntFlatPtr,
    ctypes.c_int,
    ctypes.c_int,
)
lib_i.scale_array_inplace_int.restype = IntFlatPtr

lib_i.add_array_int.argtypes = (IntFlatPtr, IntFlatPtr, ctypes.c_int)
lib_i.add_array_int.restype = IntFlatPtr

lib_i.add_array_inplace_int.argtypes = (IntFlatPtr, IntFlatPtr, ctypes.c_int)
lib_i.add_array_inplace_int.restype = IntFlatPtr

lib_i.sub_array_int.argtypes = (IntFlatPtr, IntFlatPtr, ctypes.c_int)
lib_i.sub_array_int.restype = IntFlatPtr

lib_i.sub_array_inplace_int.argtypes = (IntFlatPtr, IntFlatPtr, ctypes.c_int)
lib_i.sub_array_inplace_int.restype = IntFlatPtr

lib_i.sum_array_int.argtypes = (IntFlatPtr, ctypes.c_int)
lib_i.sum_array_int.restype = ctypes.c_int

lib_i.min_array_int.argtypes = (IntFlatPtr, ctypes.c_int)
lib_i.min_array_int.restype = ctypes.c_int

lib_i.max_array_int.argtypes = (IntFlatPtr, ctypes.c_int)
lib_i.max_array_int.restype = ctypes.c_int

lib_i.dot_arrays_int.argtypes = (IntFlatPtr, IntFlatPtr, ctypes.c_int)
lib_i.dot_arrays_int.restype = ctypes.c_int

lib_i.dot_arrays_Hadamard_int.argtypes = (
    IntFlatPtr,
    IntFlatPtr,
    ctypes.c_int,
)
lib_i.dot_arrays_Hadamard_int.restype = IntFlatPtr

lib_i.from_array_to_matrix_int.argtypes = (
    IntFlatPtr,
    ctypes.c_int,
    ctypes.c_int,
    ctypes.c_int,
)
lib_i.from_array_to_matrix_int.restype = IntRowPtr

lib_i.row_to_array_int.argtypes = (
    IntRowPtr,
    IntFlatPtr,
    ctypes.c_int,
    ctypes.c_int,
)
lib_i.row_to_array_int.restype = IntFlatPtr

lib_i.col_to_array_int.argtypes = (
    IntRowPtr,
    IntFlatPtr,
    ctypes.c_int,
    ctypes.c_int,
)
lib_i.col_to_array_int.restype = IntFlatPtr

lib_i.set_row_int.argtypes = (
    IntRowPtr,
    IntFlatPtr,
    ctypes.c_int,
    ctypes.c_int,
    ctypes.c_int,
)
lib_i.set_row_int.restype = ctypes.c_int

lib_i.set_col_int.argtypes = (
    IntRowPtr,
    IntFlatPtr,
    ctypes.c_int,
    ctypes.c_int,
    ctypes.c_int,
)
lib_i.set_col_int.restype = ctypes.c_int

lib_i.swap_rows_int.argtypes = (
    IntRowPtr,
    ctypes.c_int,
    ctypes.c_int,
    ctypes.c_int,
)
lib_i.swap_rows_int.restype = None

lib_i.T_matrix_int.argtypes = (IntRowPtr, ctypes.c_int, ctypes.c_int)
lib_i.T_matrix_int.restype = IntRowPtr

lib_i.T_matrix_inplace_int.argtypes = (IntRowPtr, ctypes.c_int, ctypes.c_int)
lib_i.T_matrix_inplace_int.restype = IntRowPtr

lib_i.dot_matrix_int.argtypes = (
    IntRowPtr,
    IntRowPtr,
    ctypes.c_int,
    ctypes.c_int,
    ctypes.c_int,
    ctypes.c_int,
)
lib_i.dot_matrix_int.restype = IntRowPtr

lib_i.sum_matrix_int.argtypes = (
    IntRowPtr,
    IntRowPtr,
    ctypes.c_int,
    ctypes.c_int,
)
lib_i.sum_matrix_int.restype = IntRowPtr

lib_i.sub_matrix_int.argtypes = (
    IntRowPtr,
    IntRowPtr,
    ctypes.c_int,
    ctypes.c_int,
)
lib_i.sub_matrix_int.restype = IntRowPtr

lib_i.dot_matrix_Hadamard_int.argtypes = (
    IntRowPtr,
    IntRowPtr,
    ctypes.c_int,
    ctypes.c_int,
)
lib_i.dot_matrix_Hadamard_int.restype = IntRowPtr

lib_i.matrix_to_array_int.argtypes = (IntRowPtr, ctypes.c_int)
lib_i.matrix_to_array_int.restype = IntFlatPtr

lib_i.cast_matrix_to_double.argtypes = (IntRowPtr, ctypes.c_int, ctypes.c_int)
lib_i.cast_matrix_to_double.restype = DoubleRowPtr


# ── Утилиты ──────────────────────────────────────────────────────────────────────
def to_list_matrix(ptr: ctypes.Array, rows: int, cols: int) -> List[List[float]]:
    return [[ptr[r][c] for c in range(cols)] for r in range(rows)]


def to_list_array(ptr: ctypes.Array, length: int) -> List[float]:
    return [ptr[i] for i in range(length)]


def fill_matrix(ptr: ctypes.Array, rows: int, cols: int, fn: Callable[[int, int], float]) -> None:
    for r in range(rows):
        for c in range(cols):
            ptr[r][c] = fn(r, c)


@contextlib.contextmanager
def redirected_stdin(text: str):
    """Подменяет stdin, чтобы C-функции читали из заранее заготовленной строки."""
    fd = sys.stdin.fileno()
    saved = os.dup(fd)
    r_fd, w_fd = os.pipe()
    try:
        os.write(w_fd, text.encode())
    finally:
        os.close(w_fd)
    os.dup2(r_fd, fd)
    os.close(r_fd)
    try:
        yield
    finally:
        os.dup2(saved, fd)
        os.close(saved)


@contextlib.contextmanager
def capture_c_stdout():
    """Перехватывает stdout C-библиотеки."""
    fd = sys.stdout.fileno()
    saved = os.dup(fd)
    r_fd, w_fd = os.pipe()
    os.dup2(w_fd, fd)
    os.close(w_fd)
    buf = bytearray()
    try:
        yield buf
        libc.fflush(None)
        os.dup2(saved, fd)
        os.close(saved)
        chunk = os.read(r_fd, 1 << 20)
        buf.extend(chunk)
    finally:
        os.close(r_fd)


def free_c_buffer(ptr) -> None:
    if ptr:
        libc.free(ptr)


# ── Демонстрации ─────────────────────────────────────────────────────────────────
def demo_core_and_wrap():
    print(">>> core.mat_new + mat_rows_f64/mat_data_f64")
    m = core.mat_new(MatType.MAT_F64, 2, 3)
    if not m:
        raise RuntimeError("mat_new failed")
    try:
        rows_ptr = core.mat_rows_f64(m)
        fill_matrix(rows_ptr, 2, 3, lambda r, c: r * 10 + c + 0.5)
        flat = core.mat_data_f64(m)
        print("rows:", to_list_matrix(rows_ptr, 2, 3))
        print("flat:", to_list_array(flat, 6))
    finally:
        core.mat_free(m)


def demo_double_scan_and_fill():
    print("\n>>> scan_matrix_double + fill_matrix_double + print_matrix_double")
    rows = ctypes.c_int()
    cols = ctypes.c_int()
    sample_input = "2 3\n1 2 3\n4 5 6\n"
    with redirected_stdin(sample_input):
        matrix = lib_d.scan_matrix_double(None, ctypes.byref(rows), ctypes.byref(cols))
    try:
        with capture_c_stdout() as buffer:
            lib_d.print_matrix_double(matrix, rows.value, cols.value)
        print(buffer.decode(), end="")

        # fill_matrix_double: перезапишем значения
        fill_input = "6 5 4 3 2 1\n"
        with redirected_stdin(fill_input):
            lib_d.fill_matrix_double(matrix, rows.value, cols.value)
        print("after fill:", to_list_matrix(matrix, rows.value, cols.value))
    finally:
        lib_d.free_matrix_double(matrix)


def demo_double_matrix_ops():
    print("\n>>> double matrix operations")
    rows, cols = 2, 3
    A = lib_d.create_matrix_double(rows, cols)
    B = lib_d.create_matrix_double(rows, cols)
    try:
        fill_matrix(A, rows, cols, lambda r, c: r + c * 0.1)
        fill_matrix(B, rows, cols, lambda r, c: (r + 1) * (c + 1))

        unit = lib_d.unit_matrix_double(cols, cols)
        try:
            print("unit:", to_list_matrix(unit, cols, cols))
        finally:
            lib_d.free_matrix_double(unit)

        copy = lib_d.copy_matrix_double(A, rows, cols)
        try:
            print("copy:", to_list_matrix(copy, rows, cols))
        finally:
            lib_d.free_matrix_double(copy)

        summed = lib_d.sum_matrix_double(A, B, rows, cols)
        subbed = lib_d.sub_matrix_double(B, A, rows, cols)
        hadamard = lib_d.dot_matrix_Hadamard_double(A, B, rows, cols)
        try:
            print("sum:", to_list_matrix(summed, rows, cols))
            print("B - A:", to_list_matrix(subbed, rows, cols))
            print("A .* B:", to_list_matrix(hadamard, rows, cols))
        finally:
            lib_d.free_matrix_double(summed)
            lib_d.free_matrix_double(subbed)
            lib_d.free_matrix_double(hadamard)

        # dot product: A (2x3) * C (3x2)
        C = lib_d.create_matrix_double(cols, rows)
        try:
            fill_matrix(C, cols, rows, lambda r, c: (r + 1) * (c + 2))
            dot = lib_d.dot_matrix_double(A, C, rows, cols, cols, rows)
            try:
                print("A dot C:", to_list_matrix(dot, rows, rows))
            finally:
                lib_d.free_matrix_double(dot)
        finally:
            lib_d.free_matrix_double(C)

        transpose = lib_d.T_matrix_double(A, rows, cols)
        try:
            print("transpose(A):", to_list_matrix(transpose, cols, rows))
        finally:
            lib_d.free_matrix_double(transpose)

        inplace = lib_d.T_matrix_inplace_double(lib_d.copy_matrix_double(A, rows, cols), rows, cols)
        try:
            print("transpose_inplace(A copy):", to_list_matrix(inplace, cols, rows))
        finally:
            lib_d.free_matrix_double(inplace)

        vec = lib_d.matrix_to_array_double(A, rows)
        print("matrix_to_array:", to_list_array(vec, rows * cols))

        arr_values = (ctypes.c_double * (rows * cols))(*range(1, rows * cols + 1))
        from_arr = lib_d.from_array_to_matrix_double(arr_values, rows * cols, rows, cols)
        try:
            print("from_array:", to_list_matrix(from_arr, rows, cols))
        finally:
            lib_d.free_matrix_double(from_arr)

        buf = (ctypes.c_double * cols)()
        lib_d.row_to_array_double(A, buf, 1, cols)
        print("row 1:", list(buf))
        lib_d.col_to_array_double(A, buf, 1, rows)
        print("col 1:", list(buf[:rows]))

        row_data = (ctypes.c_double * cols)(*[-1.0, -2.0, -3.0])
        lib_d.set_row_double(A, row_data, 0, rows, cols)
        col_data = (ctypes.c_double * rows)(*[-4.0, -5.0])
        lib_d.set_col_double(A, col_data, 2, rows, cols)
        lib_d.swap_rows_double(A, 0, 1, rows)
        print("after row/col ops:", to_list_matrix(A, rows, cols))

        sorted_matrix = lib_d.sort_matrix_double(lib_d.copy_matrix_double(A, rows, cols), rows, cols)
        try:
            print("sorted:", to_list_matrix(sorted_matrix, rows, cols))
        finally:
            lib_d.free_matrix_double(sorted_matrix)
    finally:
        lib_d.free_matrix_double(A)
        lib_d.free_matrix_double(B)


def demo_double_array_ops():
    print("\n>>> double array operations")
    length = 4
    values = (ctypes.c_double * length)(1.0, -2.0, 3.5, -4.5)
    other = (ctypes.c_double * length)(5.0, 6.0, 7.0, 8.0)

    scaled = lib_d.scale_array_double(values, 2.0, length)
    try:
        print("scale:", to_list_array(scaled, length))
    finally:
        free_c_buffer(scaled)

    lib_d.scale_array_inplace_double(other, -1.0, length)
    print("scale inplace:", list(other))

    added = lib_d.add_array_double(values, other, length)
    try:
        print("add:", to_list_array(added, length))
    finally:
        free_c_buffer(added)

    sub = lib_d.sub_array_double(values, other, length)
    try:
        print("sub:", to_list_array(sub, length))
    finally:
        free_c_buffer(sub)

    lib_d.add_array_inplace_double(values, other, length)
    print("add inplace:", list(values))
    lib_d.sub_array_inplace_double(values, other, length)
    print("sub inplace -> back:", list(values))

    print("sum:", lib_d.sum_array_double(values, length))
    print("min:", lib_d.min_array_double(values, length))
    print("max:", lib_d.max_array_double(values, length))
    print("dot:", lib_d.dot_arrays_double(values, other, length))

    had = lib_d.dot_arrays_Hadamard_double(values, other, length)
    try:
        print("hadamard:", to_list_array(had, length))
    finally:
        free_c_buffer(had)

    idx = lib_d.search_array_index_double(SearchElement.MODULE_MAX, values, 0.0, length)
    print("module max idx:", idx)


def demo_det_and_inverse():
    print("\n>>> matrix_det_lu_double")
    rows = cols = 3
    mat = lib_d.create_matrix_double(rows, cols)
    try:
        sample = [
            [2.0, 5.0, 7.0],
            [6.0, 3.0, 4.0],
            [5.0, -2.0, -3.0],
        ]
        for r in range(rows):
            for c in range(cols):
                mat[r][c] = sample[r][c]

        det_res = lib_d.matrix_det_lu_double(0, mat, rows, cols)
        print("determinant:", det_res.determinant, "status:", det_res.status, "swaps:", det_res.swaps)

        inv_res = lib_d.matrix_det_lu_double(1, mat, rows, cols)
        if inv_res.status == 0 and inv_res.invers_matrix:
            try:
                print("inverse:", to_list_matrix(inv_res.invers_matrix, rows, cols))
            finally:
                lib_d.free_matrix_double(inv_res.invers_matrix)
    finally:
        lib_d.free_matrix_double(mat)


def demo_int_matrix_ops():
    print("\n>>> int matrix operations + cast to double")
    unit = lib_i.unit_matrix_int(3, 3)
    try:
        print("unit_int:", to_list_matrix(unit, 3, 3))
    finally:
        lib_i.free_matrix_int(unit)

    rows, cols = 2, 3
    A = lib_i.create_matrix_int(rows, cols)
    B = lib_i.create_matrix_int(rows, cols)
    try:
        for r in range(rows):
            for c in range(cols):
                A[r][c] = r * 10 + c
                B[r][c] = (r + 1) * (c + 2)

        with capture_c_stdout() as buf:
            lib_i.print_matrix_int(A, rows, cols)
        print("print_matrix_int:\n" + buf.decode(), end="")

        summed = lib_i.sum_matrix_int(A, B, rows, cols)
        subbed = lib_i.sub_matrix_int(B, A, rows, cols)
        had = lib_i.dot_matrix_Hadamard_int(A, B, rows, cols)
        try:
            print("sum:", to_list_matrix(summed, rows, cols))
            print("B - A:", to_list_matrix(subbed, rows, cols))
            print("A .* B:", to_list_matrix(had, rows, cols))
        finally:
            lib_i.free_matrix_int(summed)
            lib_i.free_matrix_int(subbed)
            lib_i.free_matrix_int(had)

        row_buf = (ctypes.c_int * cols)()
        lib_i.row_to_array_int(A, row_buf, 0, cols)
        print("row 0:", list(row_buf))

        col_buf = (ctypes.c_int * rows)()
        lib_i.col_to_array_int(A, col_buf, 1, rows)
        print("col 1:", list(col_buf))

        row_set = (ctypes.c_int * cols)(*[-1, -2, -3])
        lib_i.set_row_int(A, row_set, 0, rows, cols)
        col_set = (ctypes.c_int * rows)(*[-7, -8])
        lib_i.set_col_int(A, col_set, 2, rows, cols)
        lib_i.swap_rows_int(A, 0, 1, rows)
        print("after row ops:", to_list_matrix(A, rows, cols))

        sorted_m = lib_i.sort_matrix_int(lib_i.copy_matrix_int(A, rows, cols), rows, cols)
        try:
            print("sorted:", to_list_matrix(sorted_m, rows, cols))
        finally:
            lib_i.free_matrix_int(sorted_m)

        transpose = lib_i.T_matrix_int(A, rows, cols)
        transpose_inplace = lib_i.T_matrix_inplace_int(lib_i.copy_matrix_int(A, rows, cols), rows, cols)
        try:
            print("transpose:", to_list_matrix(transpose, cols, rows))
            print("transpose inplace copy:", to_list_matrix(transpose_inplace, cols, rows))
        finally:
            lib_i.free_matrix_int(transpose)
            lib_i.free_matrix_int(transpose_inplace)

        C = lib_i.create_matrix_int(cols, rows)
        try:
            for r in range(cols):
                for c in range(rows):
                    C[r][c] = (r + 2) * (c + 1)
            dot = lib_i.dot_matrix_int(A, C, rows, cols, cols, rows)
            try:
                print("A dot C:", to_list_matrix(dot, rows, rows))
            finally:
                lib_i.free_matrix_int(dot)
        finally:
            lib_i.free_matrix_int(C)

        array_ptr = lib_i.matrix_to_array_int(A, rows)
        print("matrix_to_array_int:", to_list_array(array_ptr, rows * cols))

        arr_vals = (ctypes.c_int * (rows * cols))(*range(1, rows * cols + 1))
        from_arr = lib_i.from_array_to_matrix_int(arr_vals, rows * cols, rows, cols)
        try:
            print("from_array_int:", to_list_matrix(from_arr, rows, cols))
        finally:
            lib_i.free_matrix_int(from_arr)

        scaled = lib_i.scale_array_int(arr_vals, 2, rows * cols)
        try:
            print("scale array:", to_list_array(scaled, rows * cols))
        finally:
            free_c_buffer(scaled)

        lib_i.scale_array_inplace_int(arr_vals, -1, rows * cols)
        print("scale inplace:", list(arr_vals))

        idx = lib_i.search_array_index_int(
            SearchElement.FIRST_EQUAL,
            arr_vals,
            arr_vals[3],
            rows * cols,
        )
        print("first equal idx:", idx)

        dot_arr = lib_i.dot_arrays_int(arr_vals, arr_vals, rows * cols)
        print("dot_arrays_int:", dot_arr)

        had_arr = lib_i.dot_arrays_Hadamard_int(arr_vals, arr_vals, rows * cols)
        try:
            print("hadamard int:", to_list_array(had_arr, rows * cols))
        finally:
            free_c_buffer(had_arr)

        casted = lib_i.cast_matrix_to_double(A, rows, cols)
        try:
            print("cast to double:", to_list_matrix(casted, rows, cols))
        finally:
            lib_d.free_matrix_double(casted)
        print("sum_array_int:", lib_i.sum_array_int(arr_vals, rows * cols))
        print("min_array_int:", lib_i.min_array_int(arr_vals, rows * cols))
        print("max_array_int:", lib_i.max_array_int(arr_vals, rows * cols))

        add_int = lib_i.add_array_int(arr_vals, arr_vals, rows * cols)
        try:
            print("add_array_int:", to_list_array(add_int, rows * cols))
        finally:
            free_c_buffer(add_int)

        sub_int = lib_i.sub_array_int(arr_vals, arr_vals, rows * cols)
        try:
            print("sub_array_int:", to_list_array(sub_int, rows * cols))
        finally:
            free_c_buffer(sub_int)

        lib_i.add_array_inplace_int(arr_vals, arr_vals, rows * cols)
        lib_i.sub_array_inplace_int(arr_vals, arr_vals, rows * cols)

    finally:
        lib_i.free_matrix_int(A)
        lib_i.free_matrix_int(B)


def demo_int_scan_fill():
    print("\n>>> scan_matrix_int + fill_matrix_int")
    rows = ctypes.c_int()
    cols = ctypes.c_int()
    input_text = "2 2\n1 2\n3 4\n"
    with redirected_stdin(input_text):
        mat = lib_i.scan_matrix_int(None, ctypes.byref(rows), ctypes.byref(cols))
    try:
        print("scanned:", to_list_matrix(mat, rows.value, cols.value))
        fill_input = "9 8 7 6\n"
        with redirected_stdin(fill_input):
            lib_i.fill_matrix_int(mat, rows.value, cols.value)
        print("after fill:", to_list_matrix(mat, rows.value, cols.value))
    finally:
        lib_i.free_matrix_int(mat)


# ── Главный сценарий ─────────────────────────────────────────────────────────────
def main():
    demo_core_and_wrap()
    demo_double_scan_and_fill()
    demo_double_matrix_ops()
    demo_double_array_ops()
    demo_det_and_inverse()
    demo_int_matrix_ops()
    demo_int_scan_fill()


if __name__ == "__main__":
    main()
