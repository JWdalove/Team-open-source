#ifndef __PID_H
#define __PID_H

#include "main.h"

/* 电机堵转保护 */
typedef enum errorType_e
{
	PID_ERROR_NONE = 0x00,/* 无堵转 */
	Motor_Blocked = 0x01U/* 堵转 */
}ErrorType_e;
typedef struct
{
	uint64_t ERRORCount;
	ErrorType_e ERRORType;
}PID_ErrorHandler_t;

/* pid结构体 */
typedef struct _PID_TypeDef
{
	float Target;
	float LastNoneZeroTarget;
	float Kp;
	float Ki;
	float Kd;

	float Measure;
	float Last_Measure;
	float Err;
	float Last_Err;
	float MaxErr;

	float Pout;
	float Iout;
	float Dout;
	float ITerm;

	float Output;
	float Last_Output;

	float MaxOut;
	float IntegralLimit;
	float DeadBand;
	float ScalarA;//变积分公式参数
	float ScalarB;

	uint8_t Improve;

	PID_ErrorHandler_t ERRORHandler;

	void (*PID_param_init)(
		struct _PID_TypeDef *pid,
		uint16_t maxOut,
		uint16_t integralLimit,
		float deadband,
		float kp,
		float ki,
		float kd,
		float A,
		float B,
		uint8_t improve
	);
}PID_TypeDef;

/* pid优化功能枚举 */
typedef enum pid_Improvement_e
{
	NONE = 0x00,/* 无 */
	Integral_Limit = 0x01,/* 积分限幅 */
	Derivative_On_Measurement = 0x02,/* 微分先行 */
	Trapezoid_Intergral = 0x04,/* 梯形积分 */
	Proportional_On_Measurement = 0x08,/* 此处不涉及 */
	OutputFilter = 0x10,/* 输出滤波 */
	ChangingIntegralRate = 0x20,/* 变积分 */
	Integral_Separation = 0x40,/* 积分分离 */
	ErrorHandle = 0x80,/* 异常处理 */
}PID_Improvement_e;

extern void PID_Init(
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
);
extern void PID_Reset(PID_TypeDef *pid,float kp,float ki,float kd);
extern float PID_Calculate(PID_TypeDef *pid,float measure,float target);
#endif
