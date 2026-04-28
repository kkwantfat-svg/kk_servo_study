### 运动控制函数

| 函数                                          | 描述                     | 速度曲线 | 适用场景             |
| :-------------------------------------------- | :----------------------- | :------- | :------------------- |
| `servo_move_raw(target)`                      | 立即跳转到目标角度       | 阶跃     | 紧急定位、调试       |
| `servo_move_linear(target, duration_ms)`      | 匀速运动                 | 矩形     | 低速、不关心启停冲击 |
| `servo_move_cubic(target, duration_ms)`       | 三次缓动（速度连续）     | S形      | 日常平滑运动         |
| `servo_move_quintic(target, duration_ms)`     | 五次缓动（加速度连续）   | 高阶S形  | 对振动敏感的设备     |
| `servo_move_sine(target, duration_ms)`        | 正弦缓动                 | 正弦形   | 仿生关节、自然摆动   |
| `servo_move_trapezoidal(target, duration_ms)` | 梯形速度规划（加-匀-减） | 梯形     | 大范围、时间优先     |

所有运动函数均会：

- 记录起始角度 `start_angle` 和目标角度 `end_angle`
- 计算总步数 `total_steps = duration_ms / 10`
- 设置算法编号 `is_moving`
