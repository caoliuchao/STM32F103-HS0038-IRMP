__如果想要详细了解此种红外协议的工作模式和移植方法等，可以访问下面两个网址__  
__IRMP协议对应的网址：<https://www.mikrocontroller.net/articles/IRMP>__  
__IRSND协议对应的网址：<https://www.mikrocontroller.net/articles/IRSND>__  
  
__本工程的主控芯片是STM32F103ZET6 红外接收头是HS0038__
__在UCOSII的任务中移植了IRMP-红外多协议解码器__
__如果添加到你的过程中可能GPIO接口不太一致，需要在irmpconfig.h文件中修改__

__//红外协议的初始化__  
irmp_init();  // 初始化IRMP库,此函数内部有GPIO的初始化函数,你要按照自己的MCU更改

__//定时器的初始化__  
irmp_timer_init(); // 初始定时器需要自己修改, 系统需要一个定时器中断 周期性调用函数.

__//定时器中断__  
(void) irmp_ISR(); //把此函数放到定时器中断服务函数中.

__//接受的数据类型__    
__typedef struct IRMP_PACKED_STRUCT__  
{  
&emsp;&emsp;   uint8_t  &ensp;&emsp;&emsp;   protocol;  &emsp;&emsp;&emsp;  // protocol, e.g. NEC_PROTOCOL  
&emsp;&emsp;   uint16_t  &emsp;&emsp;     address;  &ensp; &emsp;&emsp;  // address  
&emsp;&emsp;   uint16_t  &emsp;&emsp;    command;   &emsp;&emsp;  // command  
&emsp;&emsp;   uint8_t   &ensp;&emsp;&emsp;    flags;  &ensp; &emsp;&emsp;&emsp;  // flags, e.g. repetition  
} IRMP_DATA;  

__//获取数据函数__  
IRMP_DATA  irmp_data;  
irmp_get_data (&irmp_data);