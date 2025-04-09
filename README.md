## Setup

### Step 1: Make the Script Executable
Before running the script, ensure it is executable. Run the following command in your terminal:

```bash
chmod +x generate_tpch.sh
```
### Step 2: Run the Script
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