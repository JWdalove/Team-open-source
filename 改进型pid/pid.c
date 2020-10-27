#include "pid.h"

/* pid参数存放函数 */
static void f_PID_param_init(
	PID_TypeDef *pid,
	uint16_t max_out,
	uint16_t integral_limit,
	float deadband,
	
	float kp,
	float ki,
	float kd,

	float Changing_Integral_A,
	float Changing_Integral_B,

	uint8_t improve	
)
{
	pid->DeadBand = deadband;
	pid->IntegralLimit = integral_limit;
	pid->MaxOut = max_out;
	pid->Target = 0;

	pid->Kp = kp;
	pid->Ki = ki;
	pid->Kd = kd;
	pid->ITerm = 0;

	pid->ScalarA = Changing_Integral_A;
	pid->ScalarB = Changing_Integral_B;

	pid->Improve = improve;

	pid->ERRORHandler.ERRORCount = 0;
	pid->ERRORHandler.ERRORType = PID_ERROR_NONE;

	pid->Output = 0;
}
/* 初始化函数，在使用之前先调用进行参数初始化和函数链接 */
void PID_Init(
	PID_TypeDef *pid,
	uint16_t max_out,
	uint16_t integral_limit,
	float deadband,

	float kp,
	float ki,
	float kd,

	float A,
	float B,

	uint8_t improve
)
{
	pid->PID_param_init = f_PID_param_init;/* 初始化pid各参数 */
	pid->PID_param_init(pid,max_out,integral_limit,deadband,kp,ki,kd,A,B,improve);/* 输入pid各参数 */
}
/* pid修改函数 */
void PID_Reset(PID_TypeDef *pid,float kp,float ki,float kd)
{
	pid->Kp = kp;
	pid->Ki = ki;
	pid->Kd = kd;
}
/* 梯形积分实现函数 */
static void f_Trapezoid_Integral(PID_TypeDef *pid)
{
	pid->ITerm = pid->Ki * ((pid->Err + pid->Last_Err)/2);
}
/* 微分先行实现函数 */
static void f_Derivative_On_Measurement(PID_TypeDef *pid)
{
	pid->Dout = pid->Kd * (pid->Last_Measure - pid->Measure);
}
/* 积分分离实现函数 */
static void f_Integral_Separation(PID_TypeDef *pid)
{
	//首先判断该周期内积分项是否为积累趋势
	//只有本次积分为累积趋势才会取消积分项
	if(pid->Err * pid->Iout > 0)
	{
		if(fabs(pid->Err) <= pid->MaxErr)
			return;
		else
			pid->ITerm = 0;
	}
}
/* 变速积分实现函数 */
static void f_Changing_Integral_Rate(PID_TypeDef *pid)
{
	if(pid->Err * pid->Iout > 0)
	{
		if(fabs(pid->Err) <= pid->ScalarB)
			return;/* 完整积分 */
		if(fabs(pid->Err) <= (pid->ScalarA + pid->ScalarB))
			pid->ITerm *= (pid->ScalarA - fabs(pid->Err) + pid->ScalarB) / pid->ScalarA;/* 使用线性函数过度 */
		else
			pid->ITerm = 0;/* 取消积分 */
	}
}
/* 抗饱和积分实现函数 */
static void f_Integral_Limit(PID_TypeDef *pid)
{
    float temp_Output, temp_Iout;
    temp_Iout = pid->Iout + pid->ITerm;
    temp_Output = pid->Pout + pid->Iout + pid->Dout;
    if (fabs(temp_Output) > pid->MaxOut)
    {
        if (pid->Err * pid->Iout > 0)
        {
			//在取消积分作用前，同样需要先判断当前周期内积分是否积累
            //如果积分为减小趋势则不需要限制其减小
            pid->ITerm = 0;
        }
    }
    if (temp_Iout > pid->IntegralLimit)
    {
        pid->ITerm = 0;
        pid->Iout = pid->IntegralLimit;
    }
    if (temp_Iout < -pid->IntegralLimit)
    {
        pid->ITerm = 0;
        pid->Iout = -pid->IntegralLimit;
    }
}
/* 输出限幅实现函数 */
static void f_Output_Limit(PID_TypeDef *pid)
{
	if(pid->Output > pid->MaxOut) pid->Output = pid->MaxOut;
	if(pid->Output < -pid->MaxOut) pid->Output = -pid->MaxOut;
}
/* 输出滤波实现函数 */
static void f_OutputFilter(PID_TypeDef *pid)
{
	//待写
}
/* 电机堵转控制实现函数 */
static void f_PID_ErrorHandle(PID_TypeDef *pid)
{
	//待写
	//通过检测电机温度、或进行电机堵转计数来确定电机是否堵转
	//如果堵转 设置ERRORType为Motor_Blocked
}


/* pid计算函数 */
float PID_Calculate(PID_TypeDef *pid,float measure,float target)
{
	/* 电机异常处理 */
	if(pid->Improve & ErrorHandle)
	{
		f_PID_ErrorHandle(pid);
		if(pid->ERRORHandler.ERRORType != PID_ERROR_NONE)
		{
			pid->Output = 0;
			return 0;
			//在此进行电机停止输出 或电机反转 解决堵转问题
		}
	}
	/* 误差更新 */
	pid->Measure = measure;
	pid->Target = target;
	pid->Err = pid->Target - pid->Measure;
	/* 死区外计算 */
	if(fabs(pid->Err) > pid->DeadBand)
	{
		pid->Pout = pid->Kp * pid->Err;
		pid->ITerm = pid->Ki * pid->Err;
		pid->Dout = pid->Kd * (pid->Err - pid->Last_Err);

		/* 判断是否使能积分分离 */
		if(pid->Improve & Integral_Separation)
			f_Integral_Separation(pid);
		/* 判断是否使能梯形积分 */
		if(pid->Improve & Trapezoid_Intergral) 
			f_Trapezoid_Integral(pid);
		/* 判断是否使能变积分 */
		if(pid->Improve & ChangingIntegralRate)
			f_Changing_Integral_Rate(pid);
		/* 判断是否使能积分限幅 */
		if(pid->Improve & Integral_Limit)
			f_Integral_Limit(pid);
		/* 判断是否使能微分先行 */
		if(pid->Improve & Derivative_On_Measurement)
			f_Derivative_On_Measurement(pid);

		/* 计算积分输出 */
		pid->Iout += pid->ITerm;
		/* 计算pid总输出 */
		pid->Output = pid->Pout + pid->Iout + pid->Dout;

		/* 判断是否使能输出滤波 */
		if(pid->Improve & OutputFilter)
			f_OutputFilter(pid);
		/* 输出限幅 不可删除 */
		f_Output_Limit(pid);
	}
	/* 保存数据供下一周期调用 */
	pid->Last_Measure = pid->Measure;
	pid->Last_Output = pid->Output;
	pid->Last_Err = pid->Err;
	/* 输出pid */
	return pid->Output;
}





