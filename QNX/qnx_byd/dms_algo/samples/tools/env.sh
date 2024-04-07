## Set environment for samples

if [ $# -lt 1 -o "$1" = "help" ]; then
  echo -e "Usage: \nSwitch product:"
  echo -e "source env.sh product <name>  # Switch to product, one product switch one time"

  echo -e "\nSet environment:"
  echo -e "source env.sh samples  # Set environment for samples, etc"
fi

if [ $# -gt 1 -a "$1" = "product" ]; then
  rm -rf $(pwd)/data
  cp -a "$2"/data ./
  if [ $? -eq 0 ]; then
    echo -e "\nSwitch to product: $2\n"
  else
    echo -e "\nNot find product: $2\n"
  fi

elif [ $# -gt 0 -a "$1" = "samples" ]; then
  echo -e "\nSet environment for samples, etc\n"
  export PATH=$(pwd)/samples/tools:$(pwd)/samples/bin:$PATH
  export LD_LIBRARY_PATH=$(pwd)/lib:$(pwd)/lib:$(pwd)/samples/tools/opencv/lib
  export ADSP_LIBRARY_PATH="$(pwd)/lib;/system/vendor/lib/rfsa/adsp"

else
  echo -e "\ndo nothing, for more detail, see help info.\n"
fi
