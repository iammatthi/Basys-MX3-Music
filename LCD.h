#define LCDDATA 1 // RS = 1 ; access data register
#define LCDCMD 0 // RS = 0 ; access command register
#define PMDATA PMDIN // PMP data buffer

#define LCD_Busy() LCD_Read(LCDCMD) & 0x80
#define LCD_Put(a) LCD_Write(LCDDATA, (a))
#define LCD_Cmd(a) LCD_Write(LCDCMD, (a))
#define LCD_Home() LCD_Write(LCDCMD, 2)
#define LCD_Clear() LCD_Write(LCDCMD, 1)

void LCD_Configure();
char LCD_Read(int addr);
void LCD_Write(int addr, char c);
void LCD_PutString(char *s);