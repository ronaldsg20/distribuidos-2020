
echo "" > resultados.txt

echo "|=========================|" >> resultados.txt
echo "|-------Video 720px-------|" >> resultados.txt
echo "|=========================|" >> resultados.txt

for thread in 1 2 4 6 8 10 12 14 16 18 20;
do
    echo "-------Threads: $thread -----" >> resultados.txt
    { time ./sharpen ../videos/720-sample.mp4 ./videos/720-sample-o-$thread.mp4 $thread >/dev/null 2>&1;} |&  tee -a resultados.txt
done


echo "|=========================|" >> resultados.txt
echo "|-------Video 1080px------|" >> resultados.txt
echo "|=========================|" >> resultados.txt

for thread in 1 2 4 6 8 10 12 14 16 18 20;
do
    echo "-------Threads: $thread -----" >> resultados.txt
    { time ./sharpen ../videos/1080-sample.mov ./videos/1080-sample-o-$thread.mov $thread >/dev/null 2>&1;} |&  tee -a resultados.txt
done

echo "|=========================|" >> resultados.txt
echo "|-------Video 4K----------|" >> resultados.txt
echo "|=========================|" >> resultados.txt

for thread in 1 2 4 6 8 10 12 14 16 18 20;
do
    echo "-------Threads: $thread -----" >> resultados.txt
    { time ./sharpen ../videos/4k-sample.mp4 ./videos/4k-sample-o-$thread.mp4 $thread >/dev/null 2>&1;} |&  tee -a resultados.txt
done

