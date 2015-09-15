#include "mini_ddk.h"

#pragma  INITCODE
NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver,PUNICODE_STRING str)
{
	//__asm int 3;
	JMPCODE JmpCode;

	ULONG current = GetNtCurrentAddress();// A
	ULONG source = GetNtSourceAddress();// C

	if(current != source)
	{
		JmpCode.E9 = 0xE9;
		JmpCode.JMPADDRESS = source - current - 5;
		DbgPrint("Jump Address = %0x", JmpCode.JMPADDRESS);
		DbgPrint("Hooked NtOpenProcess!");
	}

   pDriver->MajorFunction[IRP_MJ_CREATE]=ddk_DispatchRoutine_CONTROL; 
   pDriver->MajorFunction[IRP_MJ_CLOSE]=ddk_DispatchRoutine_CONTROL; 
   pDriver->MajorFunction[IRP_MJ_READ]=ddk_DispatchRoutine_CONTROL; 
   pDriver->MajorFunction[IRP_MJ_CLOSE]=ddk_DispatchRoutine_CONTROL; 
   pDriver->MajorFunction[IRP_MJ_DEVICE_CONTROL]=ddk_DispatchRoutine_CONTROL;

   createMyDevice(pDriver);

   pDriver ->DriverUnload = UnloadDriver;

   return 1;
}
#pragma PAGECODE

void UnloadDriver(PDRIVER_OBJECT pDriver)
{
	PDEVICE_OBJECT pDev;
	UNICODE_STRING symLinkName; // 

	pDev=pDriver->DeviceObject;
	IoDeleteDevice(pDev);

	RtlInitUnicodeString(&symLinkName,L"\\??\\zli888");
	IoDeleteSymbolicLink(&symLinkName);
	DbgPrint("Delete the Driver!.....\r\n");
	DbgPrint("Unloading Driver......\r\n");
}
#pragma PAGECODE

NTSTATUS ddk_DispatchRoutine_CONTROL(IN PDEVICE_OBJECT pDevobj,IN PIRP pIrp	)
{
	//对相应的IPR进行处理
	pIrp->IoStatus.Information=0;//设置操作的字节数为0，这里无实际意义
	pIrp->IoStatus.Status=STATUS_SUCCESS;//返回成功
	IoCompleteRequest(pIrp,IO_NO_INCREMENT);//指示完成此IRP
	KdPrint(("离开派遣函数\n"));//调试信息
	return STATUS_SUCCESS; //返回成功
}
