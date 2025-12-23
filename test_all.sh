#!/bin/bash
# =====================================================
# test_all.sh - Full test suite for my_ls
# Author: son
# Purpose: Compile & demonstrate all features of my_ls
# =====================================================

APP=./my_ls

echo "====================================================="
echo "  TEST SUITE: my_ls (Custom ls implementation)"
echo "====================================================="

# -----------------------------------------------------
# 1. BUILD
# -----------------------------------------------------
echo -e "\n--- [1] BIÊN DỊCH CHƯƠNG TRÌNH ---"
gcc main.c ls_core.c -o my_ls -Wall

if [ $? -ne 0 ]; then
    echo " LỖI: Biên dịch thất bại!"
    exit 1
fi
echo " Biên dịch thành công."

# -----------------------------------------------------
# 2. BASIC
# -----------------------------------------------------
echo -e "\n--- [2] KIỂM TRA CƠ BẢN ---"

echo -e "\n>> $APP (mặc định)"
$APP

echo -e "\n>> $APP -a (hiển thị file ẩn)"
$APP -a

# -----------------------------------------------------
# 3. LONG FORMAT
# -----------------------------------------------------
echo -e "\n--- [3] KIỂM TRA -l (LONG FORMAT) ---"

echo -e "\n>> $APP -l"
$APP -l

echo -e "\n>> $APP -l -a"
$APP -l -a

# -----------------------------------------------------
# 4. HUMAN READABLE
# -----------------------------------------------------
echo -e "\n--- [4] KIỂM TRA -h (HUMAN READABLE SIZE) ---"

echo -e "\n>> $APP -l -h"
$APP -l -h

echo -e "\n>> $APP -l -a -h"
$APP -l -a -h

# -----------------------------------------------------
# 5. SORT
# -----------------------------------------------------
echo -e "\n--- [5] KIỂM TRA SẮP XẾP ---"

echo -e "\n>> $APP -t (sort theo thời gian)"
$APP -t

echo -e "\n>> $APP -l -t"
$APP -l -t

echo -e "\n>> $APP -S (sort theo kích thước)"
$APP -S

echo -e "\n>> $APP -l -S"
$APP -l -S

# -----------------------------------------------------
# 6. OPTION COMBINATION
# -----------------------------------------------------
echo -e "\n--- [6] KIỂM TRA TỔ HỢP OPTION ---"

echo -e "\n>> $APP -l -a -h -t"
$APP -l -a -h -t

echo -e "\n>> $APP -l -a -h -S"
$APP -l -a -h -S

# -----------------------------------------------------
# 7. RECURSIVE
# -----------------------------------------------------
echo -e "\n--- [7] KIỂM TRA ĐỆ QUY (-R) ---"

echo -e "\n>> $APP -R"
$APP -R

echo -e "\n>> $APP -l -R"
$APP -l -R

echo -e "\n>> $APP -l -h -R"
$APP -l -h -R

echo -e "\n>> $APP -l -a -h -S -R (FULL OPTION)"
$APP -l -a -h -S -R

# -----------------------------------------------------
# 8. TEST PATH CỤ THỂ
# -----------------------------------------------------
echo -e "\n--- [8] KIỂM TRA ĐƯỜNG DẪN CỤ THỂ ---"

echo -e "\n>> $APP /tmp"
$APP /tmp

echo -e "\n>> $APP -l /tmp"
$APP -l /tmp

# -----------------------------------------------------
# 9. ERROR HANDLING
# -----------------------------------------------------
echo -e "\n--- [9] KIỂM TRA LỖI ---"

echo -e "\n>> $APP /duong/dan/khong/ton/tai"
$APP /duong/dan/khong/ton/tai

echo -e "\n>> $APP -z (option sai)"
$APP -z

# -----------------------------------------------------
# DONE
# -----------------------------------------------------
echo -e "\n====================================================="
echo " TẤT CẢ TEST ĐÃ HOÀN TẤT"
echo "====================================================="
