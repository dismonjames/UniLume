# UniLume

**Bộ gõ tiếng Việt hiện đại, nhẹ và nhanh cho Linux, phát triển trực tiếp từ lõi UniKey.**

UniLume đang ở giai đoạn khởi tạo và mang tính thử nghiệm. Repository hiện cung
cấp lõi xử lý tiếng Việt kế thừa, một API C tương thích và bộ test hồi quy nhỏ;
chưa cung cấp ứng dụng bộ gõ có thể cài đặt để sử dụng hằng ngày.

> UniLume là một dự án độc lập, được phát triển dựa trên UniKey Input Engine của Phạm Kim Long. UniLume không phải sản phẩm chính thức của UniKey và không được Phạm Kim Long đại diện hoặc bảo trợ.

## Hiện có

- Lõi UniKey/x-unikey 1.0.4 cho Telex, VNI và VIQR.
- Xuất UTF-8 và các bộ chuyển đổi bảng mã kế thừa.
- API xử lý phím, backspace, macro và keymap của x-unikey.
- Build CMake tối thiểu cho thư viện tĩnh và test engine trên Linux.
- Mã adapter XIM/GTK2 lịch sử trong `src/platform/legacy/` để tham khảo; các
  adapter này chưa nằm trong build mặc định.

## Chưa có

- Executable hoặc giao diện UniLume dành cho người dùng cuối.
- Tích hợp desktop Linux hiện đại, Wayland hoặc đóng gói distro.
- Cam kết tương thích, ổn định hay sẵn sàng cho production.

## Build và test

Yêu cầu: CMake 3.16 trở lên, compiler C++ hỗ trợ GNU C++98 và công cụ build
chuẩn trên Linux. Build mặc định không tải dependency từ Internet.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
```

Build tạo thư viện `unilume_engine` và executable test
`unilume_engine_tests`; hiện chưa có lệnh chạy ứng dụng UniLume.

Để chạy ASan và UBSan cục bộ:

```sh
cmake -S . -B build/sanitizers \
  -DCMAKE_BUILD_TYPE=Debug \
  -DUNILUME_ENABLE_ASAN=ON \
  -DUNILUME_ENABLE_UBSAN=ON
cmake --build build/sanitizers
ASAN_OPTIONS=detect_leaks=1:halt_on_error=1 \
UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1 \
ctest --test-dir build/sanitizers --output-on-failure
```

Các option sanitizer mặc định tắt và không thay thế test build thông thường.

Benchmark core được tắt mặc định. Cách build Release, chạy corpus, xuất JSON
và chạy soak được mô tả trong
[docs/benchmarks.md](docs/benchmarks.md). Các số core-only không phải phép so
sánh với Lotus, fcitx5-unikey hoặc một integration desktop hoàn chỉnh.

## Nguồn gốc và giấy phép

Phần lớn code trong `src/` và tài liệu trong `docs/legacy/` đến từ x-unikey
1.0.4/UniKey Input Engine của Phạm Kim Long. `third_party/imdkit/` là IMdkit của
X11R6 với các notice permissive riêng. Các file CMake, test và tài liệu
UniLume ở cấp repository là phần mới.

Target build hiện tại kết hợp code GPL-2.0-or-later và LGPL-2.0-or-later, nên
được phân phối theo GPL-2.0-or-later. Một số file kế thừa không có header riêng
và `COPYING` dùng tên lịch sử “GNU Library General Public License, version 2”;
xem [NOTICE](NOTICE) và [docs/licensing.md](docs/licensing.md) trước khi phát
hành binary.

`LICENSE` chứa GPL version 2; `COPYING` giữ nguyên văn bản giấy phép đi kèm
snapshot gốc. Mọi header bản quyền và điều khoản riêng của bên thứ ba vẫn có
hiệu lực.

## Ghi công

- Phạm Kim Long — tác giả UniKey và UniKey Input Engine.
- Nhóm x-unikey cùng các cộng tác viên được liệt kê trong [AUTHORS.md](AUTHORS.md).
- Hidetoshi Tajima và X11R6 Xi18n Implementation Group — IMdkit.

UniKey là tên của dự án gốc và chỉ được dùng ở đây để ghi công, mô tả nguồn
gốc kỹ thuật hoặc giữ tương thích API.

## Roadmap ngắn

1. Xác minh provenance và giấy phép các file kế thừa chưa có header.
2. Mở rộng test hồi quy mà không đổi thuật toán.
3. Thiết kế adapter Linux nhỏ, tách biệt khỏi engine.
4. Chỉ sau đó đánh giá backend desktop và packaging.

Chi tiết xem [docs/roadmap.md](docs/roadmap.md).
