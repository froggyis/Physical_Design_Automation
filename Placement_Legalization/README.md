## How to Compile  
  In this directory, enter the following command:  

```
$ make
```  
It will generate the executable files "hw4" in "HW4/bin/".  

  If you want to remove it, simply enter the following command:  
```
  $ make clean  
```
## How to Run  
  In this directory, enter the following command:  
  Usage: ../bin/<exe> <aux file> 
  e.g.:  
  $ ../bin/hw4 ../testcase/adaptec1/adaptec1.aux 

  In "HW4/bin/", enter the following command:  
  Usage: ./<exe> <aux file>  
  e.g.:  
  $ ./hw4 ../testcase/adaptec1/adaptec1.aux


## How to Verify  
  In this directory, enter the following command:  
  Usage: $ ../verifier/verify <testcase file> <result path>>  
  e.g.:  
  $ ../verifier/verify ../testcase/adaptec1/adaptec1.aux ../output/adaptec1.result 

--How to use run.sh
  In "HW4/src/", enter the following command:  
  Usage: bash run.sh 