# Branch-and-Bound Calculator in C++
Implementation of the Branch-and-Bound method for solving Integer Linear Programming problems.

Compiling and Running
----------------------
A serial executable is obtained by using your favourite C++ compiler to compile and link the downloaded files. For example, using g++:

**g++ bnb.cpp -o bnb.exe**

After done compiling, execute the 'bnb' program passing the ILP problem data file as an argument, like this:

**./bnb.exe entrada/DadosXX.esd**

If no argument is passed, the program will read the ILP problem from the user itself. Both ways will use the default search and print every node.  

Search Options
---------------
The default search is DFS(Depth-First Search) with backtracking, giving priority to '<=' constraints. But there are a few arguments that you can choose before you start the search.

**./bnb.exe entrada/DadosXX.esd ARG1 ARG2 ARG3**  

**ARG1**: Search type, (0) for DFS with backtracking, and (1) for BFS(Breadth-First Search).  
**ARG2**: Son priority, (0) for '<=', and (1) for '>=' constraints.  
**ARG3**: Specifies how often the program will print the nodes on the screen. '1000' will print every 1000 new nodes, including the root node.    

If **ARG2** or **ARG3** are not specified, the program will set priority to '<=' constraints and print every new node, respectively.  

Creating the Data
------------------
To pass the problem as an argument to the calculator, it needs to be in the specific format below, so that the Data class can read the file correctly:

>**NAME:** name of your file or problem.
>
>**p_t:** especifies the problem type -> (0) for Maximization, (1) for Minimization.  
>**n_v:** number of variables in the Objective Function (OF).  
>**n_c:** number of constraints.  
>
>**Z = XX XX XX..**    (OF)  
>**XX XX XX.. < RHS**  (Constraint 1)  
>**XX XX XX.. = RHS**  (Constraint 2)  
>**XX XX XX.. > RHS**  (Constraint 3)  

'<' is read as '<='.  
'>' is read as '>='.  
RHS holds the value of the Right-Hand Side of that constraint.  
Each XX specifies the coefficients of the variables from the OF.  
