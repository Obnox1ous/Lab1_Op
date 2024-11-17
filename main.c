#include <stdio.h>
#include <stdlib.h>

void free_matrix(double **matrix, int rows)
{
	for (int i = 0; i < rows; i++)
	{
		free(matrix[i]);
	}
	free(matrix);
}

double **allocate_matrix(int rows, int cols)
{
	double **matrix = malloc(rows * sizeof(double *));
	if (!matrix)
		return NULL;

	for (int i = 0; i < rows; i++)
	{
		matrix[i] = malloc(cols * sizeof(double));
		if (!matrix[i])
		{
			free_matrix(matrix, i);
			return NULL;
		}
	}
	return matrix;
}

void read_matrix(FILE *file, double **matrix, int rows, int cols)
{
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			fscanf(file, "%lf", &matrix[i][j]);
		}
	}
}

void print_matrix(FILE *file, double **matrix, int rows, int cols)
{
	fprintf(file, "%d %d\n", rows, cols);
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			fprintf(file, "%g ", matrix[i][j]);
		}
		fprintf(file, "\n");
	}
}

void gauss_inversion(double **matrix, double **inverse, int size)
{
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			inverse[i][j] = 1.0;
			if (i != j)
			{
				inverse[i][j] = 0.0;
			}
		}
	}

	for (int i = 0; i < size; i++)
	{
		double pivot = matrix[i][i];
		for (int j = 0; j < size; j++)
		{
			matrix[i][j] /= pivot;
			inverse[i][j] /= pivot;
		}

		for (int k = 0; k < size; k++)
		{
			if (k == i)
				continue;
			double factor = matrix[k][i];
			for (int j = 0; j < size; j++)
			{
				matrix[k][j] -= factor * matrix[i][j];
				inverse[k][j] -= factor * inverse[i][j];
			}
		}
	}
}

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
		return 1;
	}

	FILE *input = fopen(argv[1], "r");
	if (!input)
	{
		fprintf(stderr, "Error: could not open input file\n");
		return 1;
	}

	int rows, cols;
	fscanf(input, "%d %d", &rows, &cols);

	if (rows != cols)
	{
		fclose(input);
		FILE *output = fopen(argv[2], "w");
		if (output)
		{
			fprintf(output, "no_solution\n");
			fclose(output);
		}
		return 0;
	}

	double **matrix = allocate_matrix(rows, cols);
	if (!matrix)
	{
		fprintf(stderr, "Error: could not allocate memory for matrix\n");
		fclose(input);
		return 1;
	}

	read_matrix(input, matrix, rows, cols);
	fclose(input);

	double **inverse = allocate_matrix(rows, cols);
	if (!inverse)
	{
		fprintf(stderr, "Error: could not allocate memory for inverse matrix\n");
		free_matrix(matrix, rows);
		return 1;
	}

	gauss_inversion(matrix, inverse, rows);

	FILE *output = fopen(argv[2], "w");
	if (!output)
	{
		fprintf(stderr, "Error: could not open output file\n");
		free_matrix(matrix, rows);
		free_matrix(inverse, rows);
		return 1;
	}

	print_matrix(output, inverse, rows, cols);
	fclose(output);

	free_matrix(matrix, rows);
	free_matrix(inverse, rows);

	return 0;
}
