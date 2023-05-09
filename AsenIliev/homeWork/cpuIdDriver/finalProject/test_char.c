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

struct proc_dir_entry* proc_dir;

int chr_dev_register_status;


int read_from_proc = 0;




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
	    printk(KERN_ALERT"\n hello_read \n");

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
ssize_t hello_write(struct file *filp, const char __user *buffer, size_t len, loff_t * f_pos){
	printk(KERN_ALERT"\n hello_write \n");

	/*копирай съобщението от буферът към името на прозиводителят и е дадена дължината на буферът*/
	copy_from_user(manufacturer_name,buffer,len);

	return 0;
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
	printk(KERN_ALERT"\n hello_open \n");

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
	    printk(KERN_ALERT"\n hello_release \n");

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
    printk(KERN_ALERT"\nproc_open \n");
	
	if(is_proc_entry_open){
		return -EBUSY;
	}

	is_proc_entry_open = 1;

    // printk(KERN_ALERT"proc_open\n");
	return EXIT_SUCCESS;
}




ssize_t	proc_read(struct file *filp, char __user * buffer, size_t length, loff_t * f_pos){
    printk(KERN_ALERT"\nproc_read \n");

	if(read_from_proc){
		read_from_proc = 0;

		return EXIT_SUCCESS;
	}

	read_from_proc = 1;

	copy_to_user(buffer,proc_manufacturer_cpu_name_ptr,strlen(proc_manufacturer_cpu_name_ptr));

	return length;
}

ssize_t	proc_write(struct file *, const char __user *, size_t, loff_t *){
    printk(KERN_ALERT"\nproc_write\n");
    return EXIT_SUCCESS;
}

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

	


/*
Функции започващи с __ са от ниско.

Инициализационни функции трябва да са статична, тъкато не е предвидена да е видима извън файлът
например "chdr.c".
Символът __init за инициализационна функция, която функция
се ползва само по реме на инициализация.
*/
static int __init hello_init(void){




    printk(KERN_ALERT"Hello proc\n");

	
	printk(KERN_ALERT "The device number is %d",MAJOR_NUMBER);

	/*
		създаване на /proc файл
		функция proc_create връща като резултат proc_dir_entry(директория на записаният прок файл)
		параметри:

		name е името на /proc файлът

		mode  е правата върху файлът.Тъй като /proc го използвам само за четене mode ще бъде 444 

		parent показва директорията, в която /proc файлът трябва да бъде направен.
		Ако NULL то файлът ще бъде направен в /proc

		proc_ops файлови операции
	*/
    proc_dir = proc_create(PROC_NAME,0444,NULL,&proc_ops);

	/*
		Това е старият начин, по който се регистрира char driver
		register_chrdev(number,dn,fpos);

		major number ако е 0 major number ще бъде избран от текущо свободните свободните

		name името на устройството което ще бъде асоциирано с major number 		

		функцията register_chrdev връща 0 при успешна регистрация, и
		отрицателно число при неуспешна
	*/
	chr_dev_register_status =  register_chrdev(MAJOR_NUMBER, DEVICE_NAME, &fops);


	if(chr_dev_register_status < 0){
		printk(KERN_ALERT "Cant register chardev\nError : %d",chr_dev_register_status);

		proc_remove(proc_dir);	
		unregister_chrdev(MAJOR_NUMBER,DEVICE_NAME);

	}
	
	return EXIT_SUCCESS;
}

/*
Функция с която се премахва /proc/файл и char driver
Ако не бъде дефинирана такава то няма да бъде позволено да се
премахне char driver или /proc/.Тази функция трябва да отмени това което е било
направено в инициализиращата функция
*/
static void __exit hello_exit(void){
	//премахване на /proc/файлът
    proc_remove(proc_dir);	
	
	//премахване на char driver
	unregister_chrdev(MAJOR_NUMBER,DEVICE_NAME);


	printk(KERN_ALERT "Exit world\n");
}



/*
module_init функцията слъжи за инициализация и ще бъде извикана когато модулът се зареди в ядрото.
Също проверява дали ауторизиран потребитал може да зареди модулът.
*/
module_init(hello_init);

/*
module_exit функцията служи да се премахне char driver или /proc/файл.
Тя ще бъде извикана точно преди модулът да бъде премахнат от ядрото.
*/
module_exit(hello_exit);

/*
Лиценз на модълът се използва за да каже на ядрото, че той е безплатен и без
лиценз ядрото се оплаква.

GPL лицензът позволява всеки да разпространява и дори продава продъкти под този лиценз,
толкова дълго докогато има достъп до източникът и е способен да изпълнява същите права.
Ако продуктът бъде отново разпространяван то той трябва да е по GPL лиценз 
*/
MODULE_LICENSE("GPL");


//Кой е авторът
MODULE_AUTHOR("Miroslav Genov CT4872");

//какво прави
MODULE_DESCRIPTION("Creates character device driver and file in proc.Gets the message from the device driver and puts it in the proc file");