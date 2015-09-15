#include <ntddk.h>
#define INITCODE code_seg("INIT")
#define PAGECODE code_seg("PAGE")
typedef unsigned char BYTE;

typedef struct _JMPCODE
{
	//BYTE E9;
	BYTE E9;
	ULONG JMPADDRESS;
}JMPCODE, *PJMPCODE;

#pragma INITCODE
NTSTATUS createMyDevice(IN PDRIVER_OBJECT p_drive_o)
{
	NTSTATUS status;
	PDEVICE_OBJECT p_device_o;	//返回创建设备

	// 初始化名字
	UNICODE_STRING devName;
	UNICODE_STRING symLinkName;
	RtlInitUnicodeString(&devName, L"\\Device\\DDK_Device"); // '\\' => '\'

	// Create Device
	status = IoCreateDevice(
		p_drive_o,
		0,
		&devName,
		FILE_DEVICE_UNKNOWN,
		0,
		TRUE,
		&p_device_o);

	// Check
	if(!NT_SUCCESS(status))
	{
		if (status == STATUS_INSUFFICIENT_RESOURCES)
		{
			DbgPrint(("STATUS_INSUFFICIENT_RESOURCES"));
		}
		if (status == STATUS_OBJECT_NAME_EXISTS)
		{
			DbgPrint(("STATUS_OBJECT_NAME_EXISTS"));
		}
		if (status == STATUS_OBJECT_NAME_COLLISION)
		{
			DbgPrint(("STATUS_OBJECT_NAME_COLLISION"));
		}
		return status;
	}

	p_device_o->Flags |= DO_BUFFERED_IO;

	// link symbol name
	RtlInitUnicodeString(&symLinkName, L"\\??\\zli888");
	status = IoCreateSymbolicLink(&symLinkName, &devName);
	if(!NT_SUCCESS(status))
	{
		IoDeleteDevice(p_device_o);
		return status;
	}

	DbgPrint("Device has successfully created!");
	return STATUS_SUCCESS;
}

void UnloadDriver(PDRIVER_OBJECT pDriver);
NTSTATUS ddk_DispatchRoutine_CONTROL(IN PDEVICE_OBJECT pDevobj,IN PIRP pIrp	);//派遣函数

//KeServiceDescriptorTable
//extern long KeServiceDescriptorTable;

typedef struct _ServiceDescriptorTable {
	PVOID ServiceTableBase; //System Service Dispatch Table 的基地址  
	PVOID ServiceCounterTable;
	//包含着 SSDT 中每个服务被调用次数的计数器。这个计数器一般由sysenter 更新。 
	unsigned int NumberOfServices;//由 ServiceTableBase 描述的服务的数目。  
	PVOID ParamTableBase; //包含每个系统服务参数字节数表的基地址-系统服务参数表 
}*PServiceDescriptorTable;  
extern PServiceDescriptorTable KeServiceDescriptorTable;

ULONG GetNtCurrentAddress()
{
	LONG *SSDT_Adr,SSDT_NtOpenProcess_Cur_Addr,t_addr; 
	DbgPrint(("驱动成功被加载中.............................\n"));
	//读取SSDT表中索引值为0x7A的函数
	//poi(poi(KeServiceDescriptorTable)+0x7a*4)
	t_addr=(LONG)KeServiceDescriptorTable->ServiceTableBase;
	DbgPrint("Base address %0x \n",t_addr);
	SSDT_Adr=(PLONG)(t_addr+0x7A*4);
	DbgPrint("t_addr+0x7A*4=%0x \n",SSDT_Adr); 
	SSDT_NtOpenProcess_Cur_Addr=*SSDT_Adr;	
	DbgPrint("Address pointed to = %0x \n",SSDT_NtOpenProcess_Cur_Addr);

	return SSDT_NtOpenProcess_Cur_Addr;
}

ULONG GetNtSourceAddress()
{
	UNICODE_STRING src_NtOpenProcess;
	ULONG src_address;

	RtlInitUnicodeString(&src_NtOpenProcess, L"NtOpenProcess");
	src_address = (ULONG)MmGetSystemRoutineAddress(&src_NtOpenProcess);

	DbgPrint("NtOpenProcess source address = %0x", src_address);
	return src_address;
}
