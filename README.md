## Setup

### Step 1: Run the Script
You can now generate the TPC-H data by running the script.
#### Option 1: Use Default Settings
To use the default scale factor and output directory, simply run:
```bash
./generate_tpch.sh
```
#### Option 2: Specify Custom Settings
To customize the scale factor and output directory, provide them as arguments. For example:
```bash
./generate_tpch.sh <scale_factor> <output_directory>
```
<scale_factor>: The size of the dataset to generate (e.g., 0.1 for 100MB, 1 for 1GB).
<br><output_directory>: The directory where the generated data will be stored.
<br>Example:

```bash
./generate_tpch.sh 0.1 my_custom_output
```
### Step 2: Running Zettabolt

Once the TPC-H data has been generated, you can execute the `Zettabolt` program to process the data. Below is an example of how to run the program with the required arguments.

### Command Syntax
```bash
./Zettabolt --region <region_name> --start-date <start_date> --end-date <end_date> --threads <num_threads> \
            --customer <path_to_customer.tbl> \
            --orders <path_to_orders.tbl> \
            --lineitem <path_to_lineitem.tbl> \
            --supplier <path_to_supplier.tbl> \
            --nation <path_to_nation.tbl> \
            --regionfile <path_to_region.tbl> \
            --result <path_to_result_file>
```
<br>Example:
```
./Zettabolt --region ASIA --start-date 1994-01-01 --end-date 1995-01-01 --threads 4 \
            --customer ./tpch_data/customer.tbl \
            --orders ./tpch_data/orders.tbl \
            --lineitem ./tpch_data/lineitem.tbl \
            --supplier ./tpch_data/supplier.tbl \
            --nation ./tpch_data/nation.tbl \
            --regionfile ./tpch_data/region.tbl \
            --result ./tpch_data/result.txt
```
