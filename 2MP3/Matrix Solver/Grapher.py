#!/usr/bin/env python
# coding: utf-8

# In[ ]:


import matplotlib.pyplot as plt
from scipy.io import mmread
from scipy.sparse import csr_matrix

def plot_sparsity(matrix, markersize=5):
    """
    Plot sparsity of a CSR matrix.

    Parameters:
    - matrix: scipy.sparse.csr_matrix
      The CSR matrix to plot.
    - markersize: int, optional
      Size of the markers in the plot.
    """
    plt.figure(figsize=(8, 6))
    plt.spy(matrix, markersize=markersize, aspect='auto')
    plt.title('Sparsity Plot of CSR Matrix')
    plt.xlabel('Column Index')
    plt.ylabel('Row Index')
    plt.show()

# Replace file path with .mtx file
file_path = '.mtx'
csr_matrix_example = mmread(file_path).tocsr()

# Plot sparsity of the matrix
plot_sparsity(csr_matrix_example, markersize=5)

