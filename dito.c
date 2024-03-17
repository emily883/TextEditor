#include <stdlib.h> // para atexits
#include <termios.h> // funciones y mas para controlar el terminal.
#include <unistd.h> // funciones de manejo descriptores de archivos
#include <ctype.h> // pa saber q caracter es, si es numero o es letra mayuscula o minuscula y tmbn convierte mayus a minus y cositas asi :v
#include <stdio.h> // para entrada y salida estandar uwu


struct termios orig_termios; // Definir una estructura termios

void disableRawMode() { // Esta funcion restaura la configuracion original del terminal :)
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios); // restaura la config inicial que está dentro de orig_termios
}

void enableRawMode() {
  tcgetattr(STDIN_FILENO, &orig_termios); // obtiene la config actual del terminal y lo guarda en orig_termios. tcgetattr() obtienes los atributos del terminal
  atexit(disableRawMode); // registra la funcion disableRawMode para que la llame al final del programa sin importar que
  struct termios raw = orig_termios; // Crea una copia de la config original obvi, para asi poder modificar la copia sin dañar el original :)
  raw.c_lflag &= ~(ECHO | ICANON); // desactiva el eco, o sea que lo que uno escriba no se verá :), el ICANON al desactivarlo la entrada se procesa al momento en q se escribe sin presionar enter.
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw); // aplica la config modificada a la terminal
// el TCSAFLUSH asegura q el buffer de entrada y salida se limpien antes de aplicar los cambios en la configuracion del terminal :)
}

int main() {
  enableRawMode();
  char c;
  while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q');
  return 0;
}
