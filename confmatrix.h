/*************************************************************************

	Universita' degli Studi dell'Insubria

    CREATED : 20/08/2007
    AUTHOR  : Moreno Carullo - moreno.carullo@uninsubria.it
    FILE    : Confusion matrix lib header
	
	$Id: confmatrix.h 97 2008-01-09 13:11:17Z moreno $

	(!)  All functions are reported with almost no comment on input params.
	     Please see the doxygen directory for an automated Makefile.

 **************************************************************************/

#ifndef _confmatrix_h_
#define _confmatrix_h_

typedef struct {
  int *confmatrix;
  int confmatrix_sz;
  int sum;
  int diag_sum;
  int *col_sum;
  int *row_sum;
  double *pa;
  double *ua;
  double oa;
  double oa_error;
  double kappa;      /* Kappa value */
  double kappa_var;  /* Kappa variance */    
  double theta1;     /* Kappa variance theta1 */
  double theta2;     /* Kappa variance theta2 */
  double theta3;     /* Kappa variance theta3 */
  double theta4;     /* Kappa variance theta4 */
} confmatrix_t;

/** Create a new covariance matrix (CTOR)
    \param numClasses number of classes recognized by classifier
*/
confmatrix_t * confmatrix_new(int numClasses);

void confmatrix_destroy(confmatrix_t *cm);

/** Add a result to the covariance matrix.
   \param cm Confusion matrix instance.
   \param supLabel the supervised "true" label (positive integer)
   \param prevLabel the label suggested by the classifier (positive integer)
*/
void confmatrix_add_result(confmatrix_t *cm, int supLabel, int prevLabel);

/**
   \param cm Confusion matrix instance.
*/
void confmatrix_update_indexes(confmatrix_t *cm);

/** Print statistics
   \param cm Confusion matrix instance.
*/
void confmatrix_print(confmatrix_t *cm);

void confmatrix_print_latex(confmatrix_t *cm);

#endif
