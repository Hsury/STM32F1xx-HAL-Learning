实验器材:
	战舰STM32F103开发板V3版本
	
实验目的:
	学习UCOSII的任务调度,初步认识UCOSII
	
硬件资源:
	1,DS0(连接在PB5) 
	2,DS1(连接在PE5)
	
实验现象:
该代码在start_task中创建了3个任务分别为led_task, beep_task和key_task。led_task是LED0和LED1每隔500ms翻转一次。
    beep_task在没有收到删除请求的时候是隔一段时间蜂鸣器鸣叫一次，
	key_task是进行按键扫描。
	当KEY_RIGHT按键按下的时候挂起任务led_task，这是LED0和LED1停止闪烁。当KEY_LEFT按键按下的时候，如果led_task被挂起则恢复之，
	如果没有挂起则没有影响。当KEY_UP按键按下的时候删除任务beep_task。当KEY_DOWN按键按下的时候，重新创建任务beep_task。
	我们的测试顺序为：首先下载代码之后可以看到LED0和LED1不断闪烁，同时蜂鸣器不断鸣叫。这个时候我们按下KEY_RIGHT之后led_task任务被挂起，
	我们可以看到LED不再闪烁。接着我们按下KEY_LEFT，led_task任务重新恢复，可以看到LED恢复闪烁。然后我们按下KEY_UP，任务beep_task被删除，
	所以蜂鸣器不再鸣叫。这个时候我们再按下按键KEY_DOWN，任务beep_task被重新创建，所以蜂鸣器恢复鸣叫。


注意事项:
	1,本实验改自按键输入实验。
	2,本实验使用ucosii版本为V2.91。
	
 
-------------------------------------------------------------------------------------------

◆其他重要连接：
  开发板光盘资料下载地址（视频+文档+源码等）：http://www.openedv.com/posts/list/13912.htm
  添加微信公众平台方法：http://www.openedv.com/posts/list/45157.htm
  

◆友情提示：如果您想以后及时免费的收到正点原子所有开发板资料更新增加通知，请关注微信公众平台：

 2种添加方法：（动动手提升您的学习效率，惊喜不断哦）
（1）打开微信->添加朋友->公众号->输入“正点原子”->点击关注
（2）打开微信->添加朋友->输入“alientek_stm32"->点击关注

 具体微信添加方法，请参考帖子：http://www.openedv.com/posts/list/45157.htm

 

			正点原子@ALIENTEK 
			广州市星翼电子科技有限公司
			电话：020-38271790
			传真：020-36773971
			购买：http://eboard.taobao.com
			公司网站：www.alientek.com
			技术论坛：www.openedv.com
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			