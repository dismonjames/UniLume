# Đóng góp cho UniLume

UniLume dùng GitHub Issues, GitHub Projects và Pull Request để quản lý toàn bộ
công việc. Các quy tắc này áp dụng cho maintainer, collaborator và AI coding
agent.

## Quy trình công việc

Không bắt đầu code khi Issue chưa ở trạng thái `Ready`. Mỗi branch và Pull
Request chỉ xử lý một Issue.

Khi bắt đầu một Issue:

1. Assign Issue cho người thực hiện.
2. Chuyển Issue sang `In progress`.
3. Tạo branch từ `main` mới nhất.

Tên branch phải theo một trong các mẫu:

```text
feat/<issue>-<slug>
fix/<issue>-<slug>
test/<issue>-<slug>
docs/<issue>-<slug>
chore/<issue>-<slug>
```

Không push trực tiếp vào `main`. Không tự mở rộng scope; công việc phát hiện
ngoài scope phải được ghi lại trong Issue mới thay vì sửa ké. Không thêm
dependency nếu chưa giải thích nhu cầu và trade-off trong Issue.

## Giới hạn khi sửa code kế thừa

- Không bulk-format code kế thừa.
- Không sửa header bản quyền, attribution hoặc giấy phép nếu chưa được duyệt.
- Không refactor ngoài phạm vi Issue.
- Không đổi thuật toán UniKey chỉ để hiện đại hóa hoặc xử lý warning.

## Trước khi mở Pull Request

Phải:

- build sạch từ thư mục mới;
- chạy toàn bộ test;
- kiểm tra `git diff`;
- kiểm tra `git status` và file untracked;
- bảo đảm không có binary, cache, log, secret hoặc file tạm.

Pull Request phải chứa:

```text
Closes #<issue>
```

Commit message cần mô tả rõ phạm vi và kết quả; không dùng các message vô nghĩa
như “update”, “fix stuff” hoặc “changes”.

Người viết PR không tự merge PR của mình khi còn reviewer khác. Maintainer giữ
quyền quyết định merge cuối. Squash merge là phương thức mặc định.

## Định nghĩa trạng thái

### Ready

Issue chỉ được chuyển sang `Ready` khi có đủ:

- mục tiêu rõ;
- phạm vi;
- phần ngoài phạm vi;
- acceptance criteria;
- cách xác minh;
- không còn blocker chưa xử lý.

### Done

Issue chỉ được coi là `Done` khi:

- PR đã merge;
- CI xanh;
- acceptance criteria đã đạt;
- tài liệu liên quan đã cập nhật;
- không còn TODO bí mật cần hoàn thành trong cùng scope.
