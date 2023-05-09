/*
*	Resources:
*	https://tldp.org/LDP/lkmpg/2.6/html/index.html
*	https://lwn.net/Kernel/LDD3/
*/



/*
	contains a great many definitions of symbols and functions needed by load-
	able modules.

	module_init(function_name)
	module_exit(function_name)
*/
#include <linux/module.h>

/*
	for initialization and cleanup functions
	__init function_name
	__exit function_name

*/

#include <linux/init.h>

#include <linux/kernel.h>

#include <linux/fs.h>

#include <linux/version.h>

#include <asm/uaccess.h>

#include <linux/ioctl.h>

#include <linux/proc_fs.h>

#include <linux/types.h>

#include "test_char.h"

#define EXIT_SUCCESS 0
#define SUCCESS 0

/* максимална дължина на съобщението */
#define MAX_LENGH_OF_MANUFACTURER_NAME 80
/* съобщение , което ще съдържа информация за производителят на процесорът */
static char manufacturer_name[MAX_LENGH_OF_MANUFACTURER_NAME];

static int is_driver_open;	
static int is_proc_entry_open;

/* указател към символният низ на името на производителят на процесорът за char_driver файлът*/
static char *char_driver_manufacturer_cpu_name_ptr = " "; 
/*указател към символният низ на името на производителят на процесорът за proc файлът  */
static char *proc_manufacturer_cpu_name_ptr= " "; 





/*

Четене от char_driver файлът. Тази функция се активира когато файлът е отворен
и някой иска да чете от него.

Връщане
Ако стойността на съобщението е 0 то е достигнат краят на съобщението
и няма да се чете. Ако стойността не е 0 то има още дата за
четене


struct file
Репрезентира отворенят файл, който е създаден от ядрото при отваряне и се подава на 
всяка функция която оперира с файлът докато не бъде затворена и последната инстанция 
на отвореният файл, тогава ядрото освобождава структурата.

char *buffer
Буфер, в който се записва дата, която е за user space

size_t count
Рамер на поисканият трансфер


loff_t f_pos;
Това е текущата четяща или пишеща позиция.Драйверът мож да чете тази стойност ако
е необходимо да знае текущата позиция във файлът, но не е нормално да я променя.
Четенето и писането би трябвало да обнови позицията ползвайки указател, който
получават като последен аргумент, вместо да действат върху filp->f_pos директно


*/
static ssize_t hello_read(struct file *filp,char *buffer,size_t count,loff_t * f_pos){

	int bytes_written = 0;
	int bytes_to_write = 1;
	//копирай 1 символ от името на прозиводителят в буферът, върни успешено или не е копирането 
	int is_the_byte_written = copy_to_user(buffer++,char_driver_manufacturer_cpu_name_ptr++,bytes_to_write);
	
	//ако сме копирали успешно
	if(is_the_byte_written == SUCCESS){
		//присвои колко сме записали
		bytes_written = bytes_to_write;
	}


	/*
		Ако сме в краят на съобщението
		върни изходът е успешен
	*/
	if(*char_driver_manufacturer_cpu_name_ptr == 0){
		return EXIT_SUCCESS;
	}

	//върни прочетените байтове
	return bytes_written;
}


/*
	Записът - подобен е на четенето
	Записва се във char_driver файлут

	В случаят ползвам проста реализация

*/
ssize_t hello_write(struct file *filp, const char __user *buffer, size_t len, loff_t * offset){
	/*копирай съобщението от буферът към името на прозиводителят и е дадена дължината на буферът*/
	copy_from_user(manufacturer_name,buffer,len);

	return strlen(manufacturer_name);
}

/*
Тази метод се извиква когато файлът на устройството се отваря
например "cat /dev/mycharfile".
Някой от случаите в които се използва е:
инициализация,
проверка за грешки,
актуализация на f_op указател,
разпределение и запълване на стуктури

inode structure;
използва се вътрешно от ядрото за да представи файлове.И за това е различно от
fail struct , която представя отворен файлов дескриптор.Може да има много
файлови дескриптори, но всички те сочат към една inode структура.Тази структура
съдържа информация за файлът.

struct file
Репрезентира отворенят файл, който е създаден от ядрото при отваряне и се подава на 
всяка функция която оперира с файлът докато не бъде затворена и последната инстанция 
на отвореният файл, тогава ядрото освобождава структурата.
*/
static int hello_open(struct inode *inode, struct file *filp)
{

	if (is_driver_open){
		return -EBUSY;
	}
	//устройството е отворено
	is_driver_open = 1;

	// инициализирай указателите с името на производителят
	char_driver_manufacturer_cpu_name_ptr = manufacturer_name;
	proc_manufacturer_cpu_name_ptr = manufacturer_name;

	return EXIT_SUCCESS;
}




/*
Метод освобождаване.
Ролята на методът за освобождаване е обратна на методът за отваряне.
Някой от нещата за който се ползва е да се деинициализира, да се
изключи устройството
*/
static int hello_release(struct inode *inode, struct file *filp)
{
	//устройството е затворено
	is_driver_open = 0;
	
	//освободи силволните низове
	kfree(char_driver_manufacturer_cpu_name_ptr);

	return EXIT_SUCCESS;
}

/*
Входно изходен контрол.
В нашият случай се използва за да се вземе информацията
от user space.

LINUX_VERSION_CODE съдържа информация за версията на ядрото

KERNEL_VERSION(major,minor,release) служи да се създаде номер на версията, и може
да се сравни текущата версия с известният контролен пункт 

inode and filp pointers са параметри подобни на тези от метозът отвори

ioctl_num е подаден от потребител,
ioctl_param са подадени по формата на unsigned long, независимо от това да
ли са подадени от потребителят като integer or a pointer.
*/
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
 static int device_ioctl(struct inode *inode, struct file *file, unsigned int ioctl_num,unsigned long ioctl_param)
#else
  static long device_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_param)
#endif
{
	printk(KERN_ALERT"Hello from ioctl\n");
	
	switch (ioctl_num) {
		
		//в случай че поставяме съобщение
		case IOCTL_SET_MSG:
			hello_write(file, (char *)ioctl_param, strlen((char *)ioctl_param), 0);
			break;

		//в случай че взимаме съобщението от драйверът
		case IOCTL_GET_MSG:
			hello_read(file, (char *)ioctl_param, strlen((char *)ioctl_param), 0);
			break;

		//в случай че взимаме байт
		case IOCTL_GET_NTH_BYTE:
			return proc_manufacturer_cpu_name_ptr[ioctl_param];
			break;
		

		//в случай че нищо не отговар на случаите
		default:		
			//Върни код за грешка “inappropriate ioctl for device”
			return -ENOTTY;
		
	}

return EXIT_SUCCESS;
}



/*
Фундаментални операции на драйверът
По този  начин може да изпулняваме различни операции четене , запис и други.

file_operations struct
Структурата е колекция от указатели към функции.Всяко поле трябва да сочи
към указатал функция или да бъде NULL за не поддържана операция.Поведението
на ядрото когато има NULL указател за функция е различно при всяка операция.
*/
struct file_operations fops = {

	/*
		Това поле не е операция.То е указател към модулът, който е собственик
		на структурата.Полето се използва да се предотврати разтоварването на модулът
		ако дадена операция е в процес на използване.
	*/
	.owner = THIS_MODULE,
	/*
	Ползва се за да се получи датата от устройството. NULL указател връща -EINVAL (“Invalid argument”).
	Върната стйност която не е отрицателна представлява броят на успешно прочетените байтове.
	*/
	.read = hello_read,

	/*
	Ползва се за да се изпрати дата до устройството. Ако е зададен NULL указател се връща -EINVAL 
	Върната стйност която не е отрицателна представлява броят на успешно записаните байтове.
	*/
	.write = hello_write,

/*

ioctl предлага входно изходен контрол, ако е NULL указател се връща -ENOTTY.
*/
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
       .ioctl = device_ioctl,
#else
       .unlocked_ioctl = device_ioctl,
#endif	

/*
Това е първата операция която се изпулняфа от устройството.Ако не е зададена функция
няма да ни уведоми че файлът е отворен
*/
	.open = hello_open,

/*
Оперяцията се изпулнява когато struct file е освободена. Както отвори този
параметър може да е NULL
*/
	.release = hello_release
};



int	proc_open(struct inode *, struct file *){
	
	if(is_proc_entry_open){
		return -EBUSY;
	}

	is_proc_entry_open = 1;

    printk(KERN_ALERT"proc_open\n");
	return EXIT_SUCCESS;
}

// int len = 1;
ssize_t	proc_read(struct file *filp, char __user * buffer, size_t length, loff_t * offs){
    printk(KERN_ALERT"\nproc_read \n");

	int bytes_to_be_written = 1;

	int bytes_written;

	//копирай 1 символ от името на производителят в буферът
    int are_bytes_written = copy_to_user(buffer++,proc_manufacturer_cpu_name_ptr++,bytes_to_be_written);
	
	if(are_bytes_written){
		bytes_written = bytes_to_be_written;
	}

	//ако низът е равен на 0, достигнали неговият край
	if(*proc_manufacturer_cpu_name_ptr == 0){
		return EXIT_SUCCESS;
	}

	return bytes_written;

}

// ssize_t	proc_write(struct file *, const char __user *, size_t, loff_t *){
//     printk(KERN_ALERT"\nproc_write\n");
//     return EXIT_SUCCESS;
// }

int	proc_release(struct inode *, struct file *){
    printk(KERN_ALERT"\nproc_releas\n");

	is_proc_entry_open = 0;

	kfree(proc_manufacturer_cpu_name_ptr);

    return EXIT_SUCCESS;
}



/*
Файловата система на  /proc е специална, софтуерно създадена файлова система,
която се ползва от ядрото за да изнася информация.
Ползва се за четене, но може и да се пише.
В повечето случаи /proc записите са само за четен

Според мен структурата proc_ops е подобна на структура file_operations,
но има изключеня.
*/
struct proc_ops proc_ops = {
    .proc_open = proc_open,
    .proc_read = proc_read,
    .proc_write = NULL,
    .proc_release = proc_release
};

	

struct proc_dir_entry* proc_dir;
int register_status;


static int __init hello_init(void){


    printk(KERN_ALERT"Hello proc\n");

	
	printk(KERN_ALERT "The device number is %d",MAJOR_NUMBER);

    proc_dir = proc_create(PROC_NAME,0,NULL,&proc_ops);

	register_status =  register_chrdev(MAJOR_NUMBER, DEVICE_NAME, &fops);


	//TODO: if have_error_registering then unregister
	if(register_status < 0){
		printk(KERN_ALERT "Cant register chardev\nError : %d",register_status);
	}
	
	return EXIT_SUCCESS;
}


static void __exit hello_exit(void){
    proc_remove(proc_dir);	
	

	unregister_chrdev(MAJOR_NUMBER,DEVICE_NAME);


	printk(KERN_ALERT "Exit world\n");
}

module_init(hello_init);

module_exit(hello_exit);


MODULE_LICENSE("GPL");


MODULE_AUTHOR("Miroslav Genov CT4872");	/* Who wrote this module? */
MODULE_DESCRIPTION("Creates character device driver and file in proc.Gets the message from the device driver and puts it in the proc file");	/* What does this module do */

MODULE_LICENSE("GPL");


MODULE_AUTHOR("Miroslav Genov CT4872");	/* Who wrote this module? */
MODULE_DESCRIPTION("Creates character device driver and file in proc.Gets the message from the device driver and puts it in the proc file");	/* What does this module do */
