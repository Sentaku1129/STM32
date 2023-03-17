int main()
{

	//使能PB的时钟
    *(unsigned int*)(0x40021000  + 0x18) |= (1<<3);
    
    //配置PB5为通用推挽输出
    *(unsigned int*)(0x40010C00  + 0x0) &= ~(0xf<<4*5);
    *(unsigned int*)(0x40010C00  + 0x0) |= (1<<4*5);
    
    //配置PB5输出0
    *(unsigned int*)(0x40010C00  + 0x0c) &= ~(1<<5);
    //while(1);
    return 0;
}

void SystemInit()
{
    return;
}
