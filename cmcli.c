/*************************************************************************

	Universita' degli Studi dell'Insubria

    CREATED : 11/03/2007
    AUTHOR  : Moreno Carullo - moreno.carullo@uninsubria.it
    FILE    : Confusion Matrix - CLI

	$Id: cmcli.c 94 2007-12-21 16:28:27Z moreno $

    - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	
    This simple command-line tool can be used to evaluate confusion matrix
    for a given classifier able to output a simple ASCII file such as

    1 2
    1 1
    1 3

    where the first number of every line is the "truth" and the second is the
    output suggested by the classifier.

    When the EOF is reached, the confusion matrix is printed on standard output.

 **************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include "confmatrix.h"

int main(int argc, char *argv[])
{
  int classes;  /* number of classes */
  char print_type='n';
  confmatrix_t *cm;
  int t,p;  /* truth, predicted */

  /* arguments checking */
  if( argc < 2 )
  {
    printf("usage: cmcli num_of_classes [print_type]\n\n");
    printf("\tnum_of_classes\tnumber of classes of interest.\n");
    printf("\tprint_type\toptional. l for LaTeX, none for ASCII.\n\n");
    printf("\tthen the sequence of input must be provided by stdin.\n");
    return 0;
  }

  /* arguments catching */
  classes = atoi(argv[1]);
  if( argc == 3 )
  {
    print_type = argv[2][0];
  }
	
  /* creation of a new confusion matrix */
  cm = confmatrix_new( classes );
  while(!feof(stdin))
  {
	if( fscanf(stdin, "%d %d", &t, &p) == 2 )
	{
      confmatrix_add_result(cm, t, p);
    }
  }
  confmatrix_update_indexes(cm);

  switch(print_type)
  {
    case 'l':
      confmatrix_print_latex(cm);
      break;
    default:
      confmatrix_print(cm);
      break;
  }

  confmatrix_destroy(cm);

  return 0;
}
