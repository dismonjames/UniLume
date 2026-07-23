## Issue liên quan

Closes #

## Mục tiêu

Mô tả ngắn kết quả PR cần đạt được.

## Thay đổi

-

## Ngoài phạm vi

-

## Cách kiểm thử

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
```

## Kết quả kiểm thử

- [ ] Build GCC thành công
- [ ] Build Clang thành công hoặc không áp dụng
- [ ] Toàn bộ test pass
- [ ] Đã thêm hoặc cập nhật test phù hợp
- [ ] Đã kiểm tra `git diff`
- [ ] Không có binary, cache, log hoặc file tạm

## Pháp lý và provenance

- [ ] Không xóa attribution hoặc header bản quyền gốc
- [ ] Không thêm code không rõ nguồn gốc
- [ ] Không thay đổi giấy phép ngoài phạm vi được duyệt
- [ ] Không sử dụng thương hiệu UniKey như sản phẩm chính thức
- [ ] Code bên thứ ba mới có giấy phép tương thích và được ghi nhận

## Checklist reviewer

- [ ] PR chỉ xử lý một Issue
- [ ] Đúng phạm vi Issue
- [ ] Không refactor ké
- [ ] Không tạo abstraction hoặc dependency không cần thiết
- [ ] Không regression
- [ ] CI xanh
