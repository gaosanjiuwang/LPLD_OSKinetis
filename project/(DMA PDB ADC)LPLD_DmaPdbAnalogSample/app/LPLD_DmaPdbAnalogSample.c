/**
 * --------------基于"拉普兰德K60底层库V3"的工程（LPLD_DmaPdbAnalogSample）-----------------
 * @file LPLD_DmaPdbAnalogSample.c
 * @version 0.1
 * @date 2013-9-29
 * @brief 利用DMA+PDB+ADC，实现非CPU干预下的ADC采集。
 *
 * 版权所有:北京拉普兰德电子技术有限公司
 * http://www.lpld.cn
 * mail:support@lpld.cn
 * 硬件平台:  LPLD K60 Card / LPLD K60 Nano
 *
 * 本工程基于"拉普兰德K60底层库V3"开发，
 * 所有开源代码均在"lib"文件夹下，用户不必更改该目录下代码，
 * 所有用户工程需保存在"project"文件夹下，以工程名定义文件夹名，
 * 底层库使用方法见相关文档。 
 *
 */
#include "common.h"

/****************************************
 说明：
   *使用RUSH Kinetis开发板上的电位器R7
    作为模拟量调节器。
   *将MiniUSB线插入RUSH Kinetis开发板的USB
    插座，并连接至电脑USB接口。
   *使用串口调试助手波特率设置为115200
   *使用串口调试助手查看运行结果。
   *通过调节电位器的旋钮查看运行结果。
 ****************************************/

//模块初始化结构体定义
PDB_InitTypeDef pdb_init_struct;
ADC_InitTypeDef adc0_init_struct;
PIT_InitTypeDef pit0_init_struct;
DMA_InitTypeDef dma_init_struct;

uint16 Result;

//函数声明
void adc0_isr();

void main (void)
{
#if defined(CPU_MK60DZ10)  
  //1.配置ADC采样参数
  //初始化ADC0模块的A组通道
  adc0_init_struct.ADC_Adcx = ADC0;     //选择ADC0
  adc0_init_struct.ADC_BitMode = SE_12BIT;      //配置转换精度
  adc0_init_struct.ADC_CalEnable = TRUE;        //使能初始化自动校准
  adc0_init_struct.ADC_HwTrgCfg = HW_TRGA;      //配置为硬件触发转换
  adc0_init_struct.ADC_DmaEnable = TRUE;        //使能DMA
  //初始化ADC0
  LPLD_ADC_Init(adc0_init_struct);   
  //使能ADC0的DAD1输入引脚复用功能
  LPLD_ADC_Chn_Enable(ADC0, DAD1);
  //使能ADC0的DAD1输入通道、禁用转换完成中断
  LPLD_ADC_EnableConversion(ADC0, DAD1, 0, FALSE);
  
  //2.配置DMA
  //初始化PDB工作参数
  dma_init_struct.DMA_CHx = DMA_CH0;   //使用Ch0通道
  dma_init_struct.DMA_Req = ADC0_DMAREQ;        //DMA请求源为ADC0
  dma_init_struct.DMA_MajorLoopCnt = 10;        //主循环计数10次
  dma_init_struct.DMA_MinorByteCnt = 2; //次循环传输字节计数（由于ADC采样为12位，因此传输2字节）
  dma_init_struct.DMA_SourceAddr = (uint32)&(ADC0->R[0]);       //源地址：ADC0结果寄存器A地址
  dma_init_struct.DMA_SourceDataSize = DMA_SRC_16BIT;   //源地址传输数据宽度16位
  dma_init_struct.DMA_DestAddr = (uint32)&Result;       //目的地址
  dma_init_struct.DMA_DestDataSize = DMA_DST_16BIT;     //目的地址传输数据宽度16位
  dma_init_struct.DMA_AutoDisableReq = FALSE;   //禁用自动禁用请求，即不受主循环计数计数限制
  //初始化DMA
  LPLD_DMA_Init(dma_init_struct);
  //使能DMA请求
  LPLD_DMA_EnableReq(DMA_CH0);
#elif defined(CPU_MK60F12) || defined(CPU_MK60F15)  
  //1.配置ADC采样参数
  //初始化ADC2模块的A组通道
  adc0_init_struct.ADC_Adcx = ADC2;     //选择ADC2
  adc0_init_struct.ADC_BitMode = SE_12BIT;      //配置转换精度
  adc0_init_struct.ADC_CalEnable = TRUE;        //使能初始化自动校准
  adc0_init_struct.ADC_HwTrgCfg = HW_TRGA;      //配置为硬件触发转换
  adc0_init_struct.ADC_DmaEnable = TRUE;        //使能DMA
  //初始化ADC0
  LPLD_ADC_Init(adc0_init_struct);   
  //使能ADC0的DAD1输入引脚复用功能
  LPLD_ADC_Chn_Enable(ADC2, DAD0);
  //使能ADC0的DAD1输入通道、禁用转换完成中断
  LPLD_ADC_EnableConversion(ADC2, DAD0, 0, FALSE);
  
  //2.配置DMA
  //初始化PDB工作参数
  dma_init_struct.DMA_CHx = DMA_CH16;   //使用Ch16通道
  dma_init_struct.DMA_Req = ADC2_DMAREQ;        //DMA请求源为ADC2
  dma_init_struct.DMA_MajorLoopCnt = 10;        //主循环计数10次
  dma_init_struct.DMA_MinorByteCnt = 2; //次循环传输字节计数（由于ADC采样为12位，因此传输2字节）
  dma_init_struct.DMA_SourceAddr = (uint32)&(ADC2->R[0]);       //源地址：ADC0结果寄存器A地址
  dma_init_struct.DMA_SourceDataSize = DMA_SRC_16BIT;   //源地址传输数据宽度16位
  dma_init_struct.DMA_DestAddr = (uint32)&Result;       //目的地址
  dma_init_struct.DMA_DestDataSize = DMA_DST_16BIT;     //目的地址传输数据宽度16位
  dma_init_struct.DMA_AutoDisableReq = FALSE;   //禁用自动禁用请求，即不受主循环计数计数限制
  //初始化DMA
  LPLD_DMA_Init(dma_init_struct);
  //使能DMA请求
  LPLD_DMA_EnableReq(DMA_CH16); 
#endif
  //3.配置PDB触发参数
  //初始化PDB工作参数
  pdb_init_struct.PDB_CounterPeriodMs = 1000;   //PDB计数器周期设置
  pdb_init_struct.PDB_LoadModeSel = LOADMODE_0; //加载模式设置
  pdb_init_struct.PDB_ContinuousModeEnable = FALSE;     //禁用连续工作模式
  pdb_init_struct.PDB_TriggerInputSourceSel = TRIGGER_PIT0;     //配置触发源为PIT0
  //初始化PDB
  LPLD_PDB_Init(pdb_init_struct);
  //配置PDB触发ADC参数：触发ADC0模块、使能A组通道预触发、触发延时0
  LPLD_PDB_AdcTriggerCfg(ADC2, PRETRIG_EN_A, 0);
  
  //4.配置PIT触发周期
  //初始化PIT参数
  pit0_init_struct.PIT_Pitx = PIT0;     //选择PIT0
  pit0_init_struct.PIT_PeriodMs = 500;  //PIT0计数周期500ms
  //初始化PIT0
  LPLD_PIT_Init(pit0_init_struct);      
  
  while(1)
  {
    LPLD_LPTMR_DelayMs(500);
    printf("Reslut=%d\r\n", Result);
  } 
}

