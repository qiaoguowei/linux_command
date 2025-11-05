# linux基础命令和工具
```shell
grep
find
wc
ulimit
curl
scp
dos2unix/unix2dos
sed
	sed 's/原字符串/目标字符串/' 文件
	sed 's/原字符串/目标字符串/g' 文件
	sed -n '2,5 p' file.txt
awk 列处理

```
# CPU性能监控
```shell
平均负载-单位时间内，系统处于可运行状态和不可终端状态的平均进程数
uptime
 13:06:06 up 1 day,  3:33,  2 users,  load average: 0.00, 0.00, 0.00
 当前时间  系统已经运行的时间   用户总数量    1分钟，5分钟，15分钟
 当平均负载为2时
	 在只有2个cpu的系统上，意味着所有的cpu都刚好被完全占用
	 在4个cpu的系统上，cpu有50%的空闲时间
	 在只有1个cpu的系统上，有一半进程竞争不到cpu

ps
	ps -ef
	
top
mpstat
	mpstat -P ALL 1 3
Average:     CPU    %usr   %nice    %sys %iowait    %irq   %soft  %steal  %guest  %gnice   %idle
Average:     all    0.00    0.00    0.04    0.00    0.00    0.00    0.00    0.00    0.00   99.96
Average:       0    0.00    0.00    0.33    0.00    0.00    0.00    0.00    0.00    0.00   99.67
Average:       1    0.00    0.00    0.00    0.00    0.00    0.00    0.00    0.00    0.00  100.00

usr 用户态的cpu时间
nice nice值为负进程的cpu时间
sys 

pidstat
	-u

vmstat
```

# 内存性能监控
# 文件IO性能监控
# 网络IO性能监控