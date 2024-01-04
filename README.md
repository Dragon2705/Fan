Chương trình điểu khiển quạt qua UART
Thiết kế, viết chương trình điều khiển quạt thông qua cách lệnh điều khiển qua giao thức UART với các tính năng như sau:
Tắt/mở quạt (khi đang tắt thì chỉ nhận lệnh mở quạt), có LED để báo hiệu quạt đang bật hay tắt.
Tắt: Chế độ default, sau khi bị power-on reset -> quạt sẽ ở trạng thái tắt.
Mở: Quạt chỉ ở trạng thái mở sau khi nhận lệnh mở, chế độ và mức gió sẽ được giữ nguyên như trước (ví dụ trước khi tắt, quạt đang ở chế độ điều khiển bằng tay, mức gió ở mức 2, sau khi mở thì quạt sẽ giữ nguyên chế độ điều khiển bằng tay, mức gió ở mức 2)
Chế độ (có LED để báo chế độ)
Tự động: Tự điều chỉnh mức gió theo nhiệt độ đo được (nhiệt độ đọc từ bộ ADC, dải nhiệt từ 5 đến 50)
Dưới 15 đô: mức gió ở mức 0
16 đến 30 độ: mức gió ở mức 1.
Trên 30 độ: mức gió ở mức 2.
Khi quạt đang ở chế độ tự động, người dùng dùng lệch chuyển qua chế độ điều khiển bằng tay, thì mức gió được giữ nguyên như trước đó (ví dụ mức gió đang ở mức 1, dùng lệch chuyển sang chế độ điều khiển bằng tay, thì mức gió sẽ vẫn là ở mức 1).
Điều khiển bằng tay:
Người dùng dùng lệch để chọn các mức độ gió 0, 1, 2
Khi quạt đang ở chế độ tự động, mà người dùng dùng lệnh chọn mức gió - mức 1 chẳng hạn, thì quạt sẽ tự trở về chế độ điều khiển bằng tay, mức gió là 1.
Mức gió (có 3 mức gió, có LED báo hiệu đang ở mức gió nào)
Mức 0: tương đương mức duty % của PWM bằng 0 %.
Mức 1: tương đương với duty % của PWM bằng 60%.
Mức 2: tương đương với duty % của PWM bằng 80%.
Tập lệnh điều khiển được truyền qua UART với khung truyền dữ liệu được định nghĩa như sau:
STX	TxRxID	Length	Data	ETX
0x02	ID byte	Len byte	byte1	byte 2	…	…	byte n	0x03
 	 	 	 	 	 	 	 	 
STX: Start Of Text, chỉ ra việc bắt đầu một frame.
TxRxID: Chỉ ra mục đích của frame để làm gì, xem phần định nghĩa các frame bên dưới.
Length: Chỉ ra số byte data theo sau, giá trị từ 0 đến 255.
Data: Data trong frame.
ETX: End Of Text, chỉ ra việc kết thúc một frame.
Định nghĩa các frames truyền: (Từ khóa “Nhận” mang ý nghĩa là IC sẽ nhận được frame truyền qua UART)
Frame cho phép tắt/mở
Nhận
STX	0x02
TxRxID	0x01
Length	1
Data	0: Tắt
1: Mở
Giá trị khác: báo lỗi
ETX	0x03
 	 
Phản hồi
STX	0x02
TxRxID	0x81
Length	1
Data	0: Tắt hoặc mở thành công
1: Giá trị điều khiển bị sai
0xFF: Frame nhận bị lỗi, ví dụ lệnh với TxRxID = 1, nhưng giá trị nhận được của byte length = 2, sẽ báo báo lỗi này.
ETX	0x03
 	 
 
Frame cho phép chọn chế độ
Nhận
STX	0x02
TxRxID	0x02
Length	1
Data	0: Tự động
1: Điều khiển bằng tay
Giá trị khác: báo lỗi
ETX	0x03
 	 
Phản hồi
STX	0x02
TxRxID	0x82
Length	1
Data	0: Chọn đế độ thành công
1: Lỗi - giá trị điều khiển sai
2: Lỗi - Quạt đang ở trạng thái tắt
0xFF: Frame nhận bị lỗi
ETX	0x03
 
Frame cho phép chọn mức gió
Nhận
STX	0x02
TxRxID	0x03
Length	1
Data	0: Mức 0
1: Mức 1
2: Mức 2
Giá trị khác: báo lỗi
ETX	0x03
 	 
Phản hồi
STX	0x02
TxRxID	0x83
Length	1
Data	0: Chọn mức gió thành công
1: Lỗi - giá trị điều khiển sai
2: Lỗi - Quạt đang ở trạng thái tắt
0xFF: Frame nhận bị lỗi
ETX	0x03
 
Frame cho phép đọc trạng thái, chế độ, mức gió, nhiệt độ đo được
Nhận
STX	0x02
TxRxID	0x04
Length	0
Data	Không có data
ETX	0x03
 	 
Phản hồi
STX	0x02
TxRxID	0x84
Length	5 khi thành công, 1 khi bị lỗi.
Data	Byte 1: status - 0 là thành công, 0xFF là bị lỗi frame truyền.
Byte 2: trạng thái: 0 là tắt, 1 là mở
Byte 3: chế độ - 0 là tự động, 1 là điều khiển bằng tay.
Byte 4: mức gió: 0 hoặc 1 hoặc 2
 	Byte 5: nhiệt độ: từ 5 đến 50, 0x00 là nhỏ hơn 5, 0xFF là lớn hơn 50.
ETX	0x03
 
Thực hiện:
Xây dựng tài liệu thiết kế cho hệ thống.
Dùng biến trở (kết nối với ADC) để mô phỏng sự thay đổi của các giá trị nhiệt độ.
Dùng bộ PWM để điều khiển đầu ra cho mức gió.
Dùng Flash để lưu các trạng thái
Dùng GPIO để điều khiển LED
Dùng UART 1 để xây dựng giao thức điều khiển.
Phần cứng:
S32K144 evaluation board
PC
Phần mềm:
IDE: Phụ thuộc vào evaluation board được sử dụng (ví dụ: S32DS sử dụng cho S32K144 board)
Kiến thức yêu cầu:
Ngôn ngữ lập trình C/Embedded
Ngoại vi:
Kiến thức về GPIO
Kiến thức về ADC
Kiến thức về PWM
Kiến thức về Flash
Kiến thức về UART
Lộ trình thực hiện bài tập:
Thiết kế hệ thống:
Thời gian: 5 ngày làm việc
Yêu cầu: Hoàn hiện thiết kế theo mô hình 4 view +1
Trách nhiệm của mentor:
Cùng tìm hiểu với mentee về mô hình 4 view + 1.
Hỗ trợ các mentees giải dáp các thắc mắc về tool, chuẩn bị cho phase “triển khai” của mock.
Kết hợp với mentor lead, đội ngũ support, và giảng viên để hỗ trợ giải đáp các thắc mắc cho mentees.
Tham gia các meetings để theo dõi và kiểm tra trạng thái, tiến độ làm thiết kế của các mentees.
Triển khai:
Thời gian: 8 ngày làm việc
Yêu cầu: Phần mềm tối thiểu phải hoạt động được đúng theo thiết kế đưa ra trong bản thiết kế, đúng yêu cầu điều khiển. (pass tất cả các system test case mà chương trình đưa ra)
Trách nhiệm của mentor.
Hỗ trợ mentee giải quyết các vấn đề gặp phải trong quá trình triển khai code, hỗ trợ debug …etc.
Kết hợp với mentor lead, đội ngũ support, và giảng viên để đảm bảo toàn bộ phần triển khai code theo đúng design ở phase trước.
Hỗ trợ mentee kiểm tra, cài đặt các tool chuẩn bị cho phase làm “unit test”.
Tham gia các meetings để theo dõi, kiểm tra trạng thái, tiến độ code của các mentees.
Unit Test:
Thời gian: 5 ngày làm việc
Yêu cầu:
Unit test áp dụng cho các API của driver.
Đề xuất sử dụng GoogleTest (GTest).
Trách nhiệm của mentor:
Hỗ trợ các mentee các vấn đề gặp phải khi làm test.
Kết hợp với mentor lead, đội ngũ support và giảng viên để đảm bảo phần UT cover được toàn bộ các yêu cầu của mock.
Tham gia các meetings để theo dõi, kiểm tra trạng thái làm UT của các mentees.
