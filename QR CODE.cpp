#include <iostream>
#include <string>
#include "qrcodegen/QrCode.hpp"
#include <QrCode.h>
#include <QPainter>
#include <QImage>
#include <QColor>

void generateQRCode(const std::string& sha256Code, const std::string& filename) {
    qrcodegen::QrCode qrCode = qrcodegen::QrCode::encodeText(sha256Code.c_str(), qrcodegen::QrCode::Ecc::MEDIUM);
    
    constexpr int qrCodeSize = 200;  // Tamanho do QR code (em pixels)
    constexpr int borderSize = 4;    // Tamanho da borda do QR code (em módulos)
    
    QImage qrImage(qrCodeSize, qrCodeSize, QImage::Format_RGB888);
    qrImage.fill(Qt::white);  // Preenche o fundo com branco
    
    QPainter painter(&qrImage);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);  // Define a cor preta para os módulos
    
    int moduleSize = (qrCodeSize - 2 * borderSize) / qrCode.getSize();
    
    // Desenha os módulos do QR code
    for (int y = 0; y < qrCode.getSize(); ++y) {
        for (int x = 0; x < qrCode.getSize(); ++x) {
            if (qrCode.getModule(x, y)) {
                int left = borderSize + x * moduleSize;
                int top = borderSize + y * moduleSize;
                painter.drawRect(left, top, moduleSize, moduleSize);
            }
        }
    }
    
    qrImage.save(QString::fromStdString(filename), "PNG");  // Salva o QR code como arquivo PNG
}

int main() {
    std::string sha256Code = "SHA256_CODE";  // Código SHA256 da criptomoeda
    std::string filename = "qrcode.png";     // Nome do arquivo de saída do QR code
    
    generateQRCode(sha256Code, filename);
    
    std::cout << "QR code gerado com sucesso." << std::endl;
    
    return 0;
}
