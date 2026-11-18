# SysArmor

Saf C ve Assembly ile yazılmış endüstriyel düzeyde Linux sistem mühürleme ve güvenlik aracı. Bağımlılık (dependency) barındırmaz, tüm standart Linux dağıtımlarında kusursuz çalışır.

## Desteklenen Dağıtımlar
* Ubuntu / Debian / Linux Mint
* Fedora / RHEL / CentOS
* Arch Linux / Manjaro
* ve tüm POSIX uyumlu Linux çekirdekleri.

## Kurulum ve Çalıştırma

Terminali açın ve şu komutları sırasıyla yazın:

```bash
git clone [https://github.com/error0941/sysarmor.git](https://github.com/error0941/sysarmor.git)
cd sysarmor
gcc -Wall -Wextra -O2 sysarmor.c -o sysarmor
sudo ./sysarmor
