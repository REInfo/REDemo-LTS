/*!
* \file config.h
* \brief 配置需要连接的LTS环境信息
*
* 本项目是基于华宝技术LTS证券接口开发的示例程序，用于展示如何在LTS
* 环境下进行开发。示例代码演示了LTS各类接口的调用，在编写相关项目时
* 可以参考。
* 由尔易信息提供开源，最新代码可从http://github.com/REInfo获取。
* 上海尔易信息科技有限公司提供证券、期货、期权、现货等市场交易、结算、
* 风控业务的客户化定制服务。
*
* \author Christian
* \version 1.0
* \date 2014-6-16
* 
*/
#ifndef MD_CONFIG_H_
#define MD_CONFIG_H_
#pragma once

/// \brief 请求编号
/// 请求编号初始值
int requestId=0;

/// \brief 前置地址
/// 前置地址暂使用华宝证券期权仿真测试地址
char mdFront[]   ="tcp://211.144.195.163:34513";
char tradeFront[]="tcp://211.144.195.163:34505";
#endif