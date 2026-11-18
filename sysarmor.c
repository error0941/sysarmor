#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define VERSION "1.0.0-PROD"

// Kritik dosya izinlerini denetleyen ve açık kapatan endüstriyel fonksiyon
void audit_and_harden_file(const char *path) {
    // O_NOFOLLOW: Symlink (sembolik bağ) tuzaklarına karşı koruma sağlar.
    int fd = open(path, O_RDONLY | O_NOFOLLOW);
    if (fd < 0) {
        if (errno == ELOOP) {
            fprintf(stderr, "[-] GÜVENLİK UYARISI: '%s' bir sembolik bağ tuzağı barındırıyor (ELOOP)!\n", path);
        }
        return;
    }

    struct stat st;
    // fstat ile dosya tanımlayıcısı üzerinden okuma yaparak yarış durumlarını (TOCTOU) engelliyoruz
    if (fstat(fd, &st) < 0) {
        perror("[-] fstat hatası");
        close(fd);
        return;
    }

    // Yalnızca normal dosyaları işleme al
    if (S_ISREG(st.st_mode)) {
        printf("[*] İnceleniyor: %s\n", path);

        // Zafiyet Tespiti: World-Writable (Diğer kullanıcıların yazma izni var mı?)
        if (st.st_mode & S_IWOTH) {
            fprintf(stderr, "    [!] KRİTİK ZAFİYET: '%s' herkes tarafından yazılabilir (World-Writable)!\n", path);

            // Güvenli izin maskesi: Others write (S_IWOTH) iznini kaldır
            mode_t secured_mode = st.st_mode & ~S_IWOTH;

            // fchmod ile doğrudan dosya tanımlayıcısı üzerinden güvenli sıkılaştırma
            if (fchmod(fd, secured_mode) == 0) {
                printf("    [+] BAŞARILI: '%s' güvenli duruma getirildi (İzinler sıkılaştırıldı).\n", path);
            } else {
                perror("    [-] fchmod hatası");
            }
        } else {
            printf("    [güvenli] İzinler standartlara uygun.\n");
        }
    } else {
        printf("[-] Atlandı: '%s' standart bir dosya değil.\n", path);
    }

    close(fd);
}

int main(int argc, char *argv[]) {
    // Root yetkisi kontrolü (Sistem dosyalarını değiştirmek için zorunludur)
    if (getuid() != 0) {
        fprintf(stderr, "[!] HATA: Bu program sistem düzeyinde sıkılaştırma yapmak için root (sudo) yetkisi gerektirir!\n");
        fprintf(stderr, "[!] Kullanım: sudo %s\n", argv[0]);
        return 1;
    }

    printf("========================================================\n");
    printf("     SYSARMOR: Endüstriyel Linux Sıkılaştırma Motoru     \n");
    printf("     Sürüm: %s                                          \n", VERSION);
    printf("========================================================\n");

    // Denetlenecek ve sıkılaştırılacak kritik sistem dosyaları kuyruğu
    const char *critical_targets[] = {
        "/etc/passwd",
        "/etc/group",
        "/etc/crontab",
        NULL
    };

    for (int i = 0; critical_targets[i] != NULL; i++) {
        audit_and_harden_file(critical_targets[i]);
    }

    printf("========================================================\n");
    printf("[+] Denetim ve sıkılaştırma döngüsü başarıyla tamamlandı.\n");

    return 0;
}