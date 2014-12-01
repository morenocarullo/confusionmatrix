/*************************************************************************

	Universita' degli Studi dell'Insubria

    CREATED : 20/08/2007
    AUTHOR  : Moreno Carullo - moreno.carullo@uninsubria.it
    FILE    : Confusion matrix lib
	
	$Id: confmatrix.c 97 2008-01-09 13:11:17Z moreno $
	
    NOTE: Unit tests for this module are in test-confmatrix.c

	(!)  All functions are reported with almost no comment on input params.
	     Please refer to confmatrix.h for a complete Doxygen-able documentation.
	     Please see the doxygen directory for an automated Makefile.

 **************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "confmatrix.h"

#define M_IDX(R,C,MAX) ((R)*(MAX) + (C))

/*
  Create a new covariance matrix
*/
confmatrix_t * confmatrix_new(int numClasses)
{
  confmatrix_t * cm;
  int i;

  cm = (confmatrix_t *)malloc(sizeof(confmatrix_t));
  cm->col_sum = (int *)malloc(sizeof(int) * numClasses);
  cm->row_sum = (int *)malloc(sizeof(int) * numClasses);
  cm->pa      = (double *)malloc(sizeof(double) * numClasses);
  cm->ua      = (double *)malloc(sizeof(double) * numClasses);
  cm->confmatrix_sz = numClasses;
  cm->confmatrix = (int *)calloc(numClasses*numClasses, sizeof(int));
  for(i=0; i<cm->confmatrix_sz*cm->confmatrix_sz; i++) {
    cm->confmatrix[i] = 0;
  }
  return cm;
}

/*
  Destroy Confusion matrix instance
*/
void confmatrix_destroy(confmatrix_t *cm)
{
  free(cm->col_sum);
  free(cm->row_sum);
  free(cm->pa);
  free(cm->ua);
  free(cm);
}

/*
  Add a result to the covariance matrix.
*/
void confmatrix_add_result(confmatrix_t *cm, int supLabel, int prevLabel)
{
  if( cm->confmatrix == NULL || supLabel<1 || prevLabel<1 ||
      supLabel>cm->confmatrix_sz || prevLabel>cm->confmatrix_sz) return;

  cm->confmatrix[M_IDX(prevLabel-1, supLabel-1, cm->confmatrix_sz)]++;
}

/*
  Update Stats & Indexes.
*/
void confmatrix_update_indexes(confmatrix_t *cm)
{
  int i,j;
  int kappa_rc=0;

  cm->sum = 0;
  cm->diag_sum = 0;

  for(i=0; i<cm->confmatrix_sz; i++) {
    cm->col_sum[i] = 0;
    cm->row_sum[i] = 0;
  }

  for(i=0; i<cm->confmatrix_sz; i++) {
    for(j=0; j<cm->confmatrix_sz; j++) {
      cm->row_sum[i] += cm->confmatrix[M_IDX(i,j,cm->confmatrix_sz)];
      cm->col_sum[j] += cm->confmatrix[M_IDX(i,j,cm->confmatrix_sz)];
    }
    cm->pa[i] = cm->row_sum[i]>0 ? (double)cm->confmatrix[M_IDX(i,i,cm->confmatrix_sz)]/cm->row_sum[i] : 0;
    cm->sum  += cm->row_sum[i];
  }

  for(j=0; j<cm->confmatrix_sz; j++) {
    cm->ua[j] = cm->col_sum[j]>0 ? (double)cm->confmatrix[M_IDX(j,j,cm->confmatrix_sz)]/cm->col_sum[j] : 0;
  }

  for(i=0, cm->diag_sum = 0; i<cm->confmatrix_sz; i++) {
    cm->diag_sum += cm->confmatrix[M_IDX(i,i,cm->confmatrix_sz)];
  }
  
  /* oa */
  cm->oa = (double)cm->diag_sum/cm->sum;
  cm->oa_error = sqrt( 1.96 * cm->oa * (1-cm->oa) / cm->sum  );

  /* kappa value */
  for(i=0, kappa_rc=0; i<cm->confmatrix_sz; i++) {
    kappa_rc += cm->col_sum[i] * cm->row_sum[i];
  }

  cm->kappa = 0.0;
  if( kappa_rc > 0 )
  {
    cm->kappa = (double)100 * (((cm->sum * cm->diag_sum) - kappa_rc) / (double)((cm->sum * cm->sum) - kappa_rc));
  }


  /* kappa variance */
  cm->kappa_var = 0.0;
  cm->theta1 = 0.0;
  cm->theta2 = 0.0;
  cm->theta3 = 0.0;
  cm->theta4 = 0.0;
  if( cm->kappa != 0.0)
  {  
    cm->theta1 = (double)cm->diag_sum/cm->sum;
  
    cm->theta2 = 0;
    for(i=0; i<cm->confmatrix_sz; i++) {
      cm->theta2 += cm->col_sum[i]*cm->row_sum[i];         
    }
    cm->theta2 /= cm->sum * cm->sum;
  
    cm->theta3 = 0;
    for(i=0; i<cm->confmatrix_sz; i++) {
      cm->theta3 += cm->confmatrix[M_IDX(i,i,cm->confmatrix_sz)] *
                    (cm->row_sum[i] + cm->col_sum[i]);        
    }
    cm->theta3 /= cm->sum * cm->sum;
  
    cm->theta4 = 0;
    for(i=0; i<cm->confmatrix_sz; i++) {
      for(j=0; j<cm->confmatrix_sz; j++) {
        cm->theta4 += cm->confmatrix[M_IDX(i,j,cm->confmatrix_sz)] *
                      pow(cm->row_sum[i] + cm->col_sum[i], 2);        
      }
    }
    cm->theta4 /= cm->sum * cm->sum * cm->sum;
 
	cm->kappa_var += (cm->theta1*(1-cm->theta1)/pow(1-cm->theta2,2));
	cm->kappa_var += 2*(1-cm->theta1)*(2*cm->theta1*cm->theta2-cm->theta3)/pow((1-cm->theta2),3);
	cm->kappa_var += pow(1-cm->theta1,3)*(cm->theta4-4*pow(cm->theta2,2))/pow(1-cm->theta2,4);
	cm->kappa_var /= cm->sum;
  }
}

/*
  Print statistics
  NOTE: portions for "Kappa" are from ConfusionMatrix.java by Ignazio Gallo

  Kappa variance is implemented by Moreno Carullo, and can be used
  to evaluate the Z-Test index.
*/
void confmatrix_print(confmatrix_t *cm)
{
  int i, j;

  /* header */
  printf("  |");
  for(i=0; i<cm->confmatrix_sz; i++) { printf("%6d|", i+1); }
  printf(" Tot  |  UA  |\n--+");
  for(i=0; i<cm->confmatrix_sz; i++)  printf("------+");
  printf("------+------+\n");

  /* confusion matrix */
  for(i=0; i<cm->confmatrix_sz; i++) {
    printf("%2d|", i+1);    
    for(j=0; j<cm->confmatrix_sz; j++) {
      printf("%6d|", cm->confmatrix[M_IDX(i,j,cm->confmatrix_sz)]);
    }
    printf("%6d|", cm->row_sum[i]);
    printf("%6.2f %%\n", cm->pa[i]*100);
  }

  /* totals */
  printf("--+");
  for(i=0; i<cm->confmatrix_sz; i++)  printf("------+");
  printf("------+------+\n");
  printf("T.|");
  for(j=0; j<cm->confmatrix_sz; j++) {
    printf("%6d|", cm->col_sum[j]);
  }
  printf("\n");
  printf("PA|");
  for(j=0; j<cm->confmatrix_sz; j++) {
    printf("%6.2f|", cm->ua[j]*100);
  }
  printf("\n");
  
  /* total accuracy and error */
  printf("\n");
  printf("Overall Accuracy (OA): %.4lf%% +/- %.2lf%% (%d hit, %d miss, %d total)\n",
         100*cm->oa, 100*cm->oa_error, cm->diag_sum, cm->sum-cm->diag_sum, cm->sum);  
  printf("Generalization Error (1-OA): %.4lf %%\n", 100*(double)(cm->sum - cm->diag_sum)/cm->sum);

  /* kappa */  
  printf("KAPPA value: %.4lf %%\n", cm->kappa);
  printf("KAPPA std.err: %.4lf \n", cm->kappa_var);
  
}

/*
  Print Confusion matrix & stats for LaTeX output.
*/
void confmatrix_print_latex(confmatrix_t *cm)
{
  int i, j;

  /* header */
  printf("%% requires booktabs package!");
  printf("\\begin{table}[htb]\n");
  printf("\\begin{center}\n");
  printf("\\begin{tabular}{@{}");
  for(i=0; i<cm->confmatrix_sz+3; i++) { printf("c"); }
  printf("@{}}\n");
  printf("\\toprule\n");
  printf("- ");

  printf("  &");
  for(i=0; i<cm->confmatrix_sz; i++) { printf("$\\omega_{%d}$ &", i+1); }
  printf(" Tot U &  UA  \\\\\n");
  printf("\\midrule\n");

  /* confusion matrix */
  for(i=0; i<cm->confmatrix_sz; i++) {
    printf("$\\omega_{%d}$ &", i+1);    
    for(j=0; j<cm->confmatrix_sz; j++) {
      printf("%6d &", cm->confmatrix[M_IDX(i,j,cm->confmatrix_sz)]);
    }
    printf("%6d &", cm->row_sum[i]);
    printf("%6.2f \\%% \\\\\n", cm->pa[i]*100);
    printf("\\hline\n");
  }

  /* totals */
  printf("Tot P &");
  for(j=0; j<cm->confmatrix_sz; j++) {
    printf("%6d &", cm->col_sum[j]);
  }
  printf("- & - \\\\ \n");
  printf("\\hline\n");
  printf("PA &");
  for(j=0; j<cm->confmatrix_sz; j++) {
    printf("%6.2f \\%% &", cm->ua[j]*100);
  }
  printf("- & - \\\\\n");
  printf("\\midrule\n");
  
  /* total accuracy and error */
  printf("\\multicolumn{%d}{@{}l@{}}{ Total accuracy: %.4lf \\%% (%d hit, %d miss, %d total) } \\\\ \n",
         cm->confmatrix_sz+3,
         100*(double)cm->diag_sum/cm->sum, cm->diag_sum, cm->sum-cm->diag_sum, cm->sum);
  printf("\\multicolumn{%d}{@{}l@{}}{ Total error: %.4lf \\%% } \\\\ \n",
         cm->confmatrix_sz+3,
         100*(double)(cm->sum - cm->diag_sum)/cm->sum);
  
  printf("\\multicolumn{%d}{@{}l@{}}{  KAPPA value: %.4lf \\%% } \\\\ \n", cm->confmatrix_sz+3, cm->kappa);
  printf("\\multicolumn{%d}{@{}l@{}}{  KAPPA std.err: %.4lf } \\\\ \n", cm->confmatrix_sz+3, cm->kappa_var);
  printf("\\bottomrule\n");
  printf("\\end{tabular}\n");
  printf("\\end{center}\n");
  printf("\\label{INSERT_LABEL_HERE}\n");
  printf("\\caption{INSERT_CAPTION_HERE}\n");
  printf("\\end{table}%%\n");
  
}
