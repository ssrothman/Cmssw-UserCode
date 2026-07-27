
for dataset in $(cat datasets_genonly.txt); do
    echo $dataset
    python setup_production.py ../../test/production_genonly_res4.py $dataset --submit
done