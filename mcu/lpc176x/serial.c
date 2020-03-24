#define LPC_UART		LPC_UART0
#define UART_IRQn		UART0_IRQn

#define RT_UART_RX_BUFFER_SIZE		64

#define IER_RBR		0x01
#define IER_THRE	0x02
#define IER_RLS		0x04

#define IIR_PEND	0x01
#define IIR_RLS		0x03
#define IIR_RDA		0x02
#define IIR_CTI		0x06
#define IIR_THRE	0x01

#define LSR_RDR		0x01
#define LSR_OE		0x02
#define LSR_PE		0x04
#define LSR_FE		0x08
#define LSR_BI		0x10
#define LSR_THRE	0x20
#define LSR_TEMT	0x40
#define LSR_RXFE	0x80


struct rt_uart_lpc
{
	struct rt_device parent;

	/* buffer for reception */
	rt_uint8_t read_index, save_index;
	rt_uint8_t rx_buffer[RT_UART_RX_BUFFER_SIZE];
}uart_device;


void arch_Uart0Handler(void)
{
	rt_ubase_t level;
    struct rt_uart_lpc* uart = &uart_device;

    /* read IIR and clear it */
	if ((LPC_UART->IIR & IIR_PEND) == 0) {
		while (LPC_UART->LSR & LSR_RDR)
		{
			/* Receive Data Available */
	        uart->rx_buffer[uart->save_index] = LPC_UART->RBR;
	
	        level = rt_hw_interrupt_disable();
			uart->save_index ++;
	        if (uart->save_index >= RT_UART_RX_BUFFER_SIZE)
	            uart->save_index = 0;
	        rt_hw_interrupt_enable(level);
	
			/* invoke callback */
			if(uart->parent.rx_indicate != RT_NULL)
			{
			    rt_size_t length;
			    if (uart->read_index > uart->save_index)
	                length = RT_UART_RX_BUFFER_SIZE - uart->read_index + uart->save_index;
	            else
	                length = uart->save_index - uart->read_index;
	
	            uart->parent.rx_indicate(&uart->parent, length);
			}
		}
	}

	if (LPC_UART->IER & IER_THRE)
		LPC_UART->IER &= ~IER_THRE;
}


static rt_err_t rt_uart_init (rt_device_t dev)
{

	return RT_EOK;
}

static rt_err_t rt_uart_open(rt_device_t dev, rt_uint16_t oflag)
{

	RT_ASSERT(dev != RT_NULL);
	if (dev->flag & RT_DEVICE_FLAG_INT_RX)
	{
		/* Enable the UART Interrupt */
		NVIC_EnableIRQ(UART_IRQn);
	}

	return RT_EOK;
}

static rt_err_t rt_uart_close(rt_device_t dev)
{

	RT_ASSERT(dev != RT_NULL);
	if (dev->flag & RT_DEVICE_FLAG_INT_RX)
	{
		/* Disable the UART Interrupt */
		NVIC_DisableIRQ(UART_IRQn);
	}

	return RT_EOK;
}

static rt_size_t rt_uart_read(rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
	rt_uint8_t* ptr;
	struct rt_uart_lpc *uart = (struct rt_uart_lpc*)dev;
	RT_ASSERT(uart != RT_NULL);

	/* point to buffer */
	ptr = (rt_uint8_t*) buffer;
	if (dev->flag & RT_DEVICE_FLAG_INT_RX)
	{
		while (size)
		{
			/* interrupt receive */
			rt_base_t level;

			/* disable interrupt */
			level = rt_hw_interrupt_disable();
			if (uart->read_index != uart->save_index)
			{
				*ptr = uart->rx_buffer[uart->read_index];

				uart->read_index ++;
				if (uart->read_index >= RT_UART_RX_BUFFER_SIZE)
					uart->read_index = 0;
			}
			else
			{
				/* no data in rx buffer */

				/* enable interrupt */
				rt_hw_interrupt_enable(level);
				break;
			}

			/* enable interrupt */
			rt_hw_interrupt_enable(level);

			ptr ++;
			size --;
		}

		return (rt_uint32_t)ptr - (rt_uint32_t)buffer;
	}

	return 0;
}

static rt_size_t rt_uart_write(rt_device_t dev, rt_off_t pos, const void* buffer, rt_size_t size)
{
	char *ptr;
	ptr = (char*)buffer;

	if (dev->flag & RT_DEVICE_FLAG_STREAM)
	{
		/* stream mode */
		while (size)
		{
			if (*ptr == '\n')
			{
				/* THRE status, contain valid data */
				while ( !(LPC_UART->LSR & LSR_THRE) );
				/* write data */
				LPC_UART->THR = '\r';
			}

			/* THRE status, contain valid data */
			while ( !(LPC_UART->LSR & LSR_THRE) );	
			/* write data */
			LPC_UART->THR = *ptr;
			
			ptr ++;
			size --;
		}
	}
	else
	{
		while ( size != 0 )
		{
			/* THRE status, contain valid data */
			while ( !(LPC_UART->LSR & LSR_THRE) );
	
			/* write data */
			LPC_UART->THR = *ptr;
	
			ptr++;
			size--;
		}
	}

	return (rt_size_t) ptr - (rt_size_t) buffer;
}



/* init console to support rt_kprintf */
void rt_hw_console_init(void)
{
	struct rt_uart_lpc* uart;

	/* get uart device */
	uart = &uart_device;

	/* device initialization */
	uart->parent.type = RT_Device_Class_Char;
	rt_memset(uart->rx_buffer, 0, sizeof(uart->rx_buffer));
	uart->read_index = uart->save_index = 0;

	/* device interface */
	uart->parent.init		= rt_uart_init;
	uart->parent.open		= rt_uart_open;
	uart->parent.close		= rt_uart_close;
	uart->parent.read		= rt_uart_read;
	uart->parent.write		= rt_uart_write;
	uart->parent.control	= RT_NULL;
	uart->parent.user_data	= RT_NULL;

	rt_device_register(&uart->parent,
		"uart1", RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_STREAM | RT_DEVICE_FLAG_INT_RX);
}




