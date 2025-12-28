#include "rc522.h"
#include <string.h>

extern SPI_HandleTypeDef hspi1;

// GPIO定义（根据您的配置）
#define RC522_NSS_Pin      GPIO_PIN_4
#define RC522_NSS_GPIO_Port GPIOA
#define RC522_RST_Pin      GPIO_PIN_0
#define RC522_RST_GPIO_Port GPIOA

// RC522寄存器地址
#define CommandReg         0x01
#define ComIEnReg          0x02
#define DivIEnReg          0x03
#define ComIrqReg          0x04
#define DivIrqReg          0x05
#define ErrorReg           0x06
#define Status1Reg         0x07
#define Status2Reg         0x08
#define FIFODataReg        0x09
#define FIFOLevelReg       0x0A
#define WaterLevelReg      0x0B
#define ControlReg         0x0C
#define BitFramingReg      0x0D
#define CollReg            0x0E
#define ModeReg            0x11
#define TxModeReg          0x12
#define RxModeReg          0x13
#define TxControlReg       0x14
#define TxASKReg           0x15
#define TxSelReg           0x16
#define RxSelReg           0x17
#define RxThresholdReg     0x18
#define DemodReg           0x19
#define MfTxReg            0x1C
#define MfRxReg            0x1D
#define SerialSpeedReg     0x1F
#define CRCResultRegH      0x21
#define CRCResultRegL      0x22
#define ModWidthReg        0x24
#define RFCfgReg           0x26
#define GsNReg             0x27
#define CWGsPReg           0x28
#define ModGsPReg          0x29
#define TModeReg           0x2A
#define TPrescalerReg      0x2B
#define TReloadRegH        0x2C
#define TReloadRegL        0x2D
#define TCounterValueRegH  0x2E
#define TCounterValueRegL  0x2F
#define TestSel1Reg        0x31
#define TestSel2Reg        0x32
#define TestPinEnReg       0x33
#define TestPinValueReg    0x34
#define TestBusReg         0x35
#define AutoTestReg        0x36
#define VersionReg         0x37
#define AnalogTestReg      0x38
#define TestDAC1Reg        0x39
#define TestDAC2Reg        0x3A
#define TestADCReg         0x3B

// 私有函数
static void RC522_Select(void);
static void RC522_Unselect(void);
static uint8_t RC522_SPI_Transfer(uint8_t data);
static void RC522_Write(uint8_t addr, uint8_t val);
static uint8_t RC522_Read(uint8_t addr);

// 选择RC522
static void RC522_Select(void) {
    HAL_GPIO_WritePin(RC522_NSS_GPIO_Port, RC522_NSS_Pin, GPIO_PIN_RESET);
}

// 取消选择RC522
static void RC522_Unselect(void) {
    HAL_GPIO_WritePin(RC522_NSS_GPIO_Port, RC522_NSS_Pin, GPIO_PIN_SET);
}

// SPI传输
static uint8_t RC522_SPI_Transfer(uint8_t data) {
    uint8_t rx_data;
    HAL_SPI_TransmitReceive(&hspi1, &data, &rx_data, 1, 100);
    return rx_data;
}

// 写寄存器
static void RC522_Write(uint8_t addr, uint8_t val) {
    RC522_Select();
    RC522_SPI_Transfer((addr << 1) & 0x7E);  // 写命令：地址左移1位，bit0=0
    RC522_SPI_Transfer(val);
    RC522_Unselect();
}

// 读寄存器
static uint8_t RC522_Read(uint8_t addr) {
    uint8_t val;
    RC522_Select();
    RC522_SPI_Transfer(((addr << 1) & 0x7E) | 0x80);  // 读命令：bit0=1
    val = RC522_SPI_Transfer(0x00);
    RC522_Unselect();
    return val;
}

// RC522初始化
// RC522初始化
void RC522_Init(void) {
    HAL_Delay(50);
    
    // 复位RC522
    HAL_GPIO_WritePin(RC522_RST_GPIO_Port, RC522_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(10);  // 延长复位时间
    HAL_GPIO_WritePin(RC522_RST_GPIO_Port, RC522_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(50);
    
    RC522_Unselect();
    HAL_Delay(5);
    
    // 复位RC522寄存器
    RC522_Write(CommandReg, PCD_RESETPHASE);
    HAL_Delay(5);
    
    // 等待复位完成
    while (RC522_Read(CommandReg) & 0x10) {
        HAL_Delay(1);
    }
    
    // === 使用标准配置 ===
    // 1. 定时器配置
    RC522_Write(TModeReg, 0x8D);        // 定时器自动重启
    RC522_Write(TPrescalerReg, 0x3E);   // 定时器分频
    RC522_Write(TReloadRegL, 30);       // 重载值低字节
    RC522_Write(TReloadRegH, 0);        // 重载值高字节
    
    // 2. 传输模式
    RC522_Write(TxModeReg, 0x00);       // 发送模式
    RC522_Write(RxModeReg, 0x00);       // 接收模式
    
    // 3. 调制宽度
    RC522_Write(ModWidthReg, 0x26);
    
    // 4. 其他配置
    RC522_Write(TxASKReg, 0x40);        // 100% ASK调制
    RC522_Write(ModeReg, 0x3D);         // CRC初始值0x6363
    
    // 5. 天线开启 - 标准方式
    RC522_SetBitMask(TxControlReg, 0x03);
    
    // 6. 其他建议设置
    RC522_Write(RFCfgReg, 0x4F);        // 接收增益48dB
    RC522_Write(RxThresholdReg, 0x85);  // 接收阈值
    RC522_Write(DemodReg, 0x4D);        // 解调器设置
    
    HAL_Delay(10);
    
    // 验证RC522版本
    uint8_t version = RC522_Read(VersionReg);
    
    // 版本检查（可选调试）
    // 正常应该是0x92或0x91
}

// 设置位掩码
void RC522_SetBitMask(uint8_t reg, uint8_t mask) {
    uint8_t tmp;
    tmp = RC522_Read(reg);
    RC522_Write(reg, tmp | mask);
}

// 清除位掩码
void RC522_ClearBitMask(uint8_t reg, uint8_t mask) {
    uint8_t tmp;
    tmp = RC522_Read(reg);
    RC522_Write(reg, tmp & (~mask));
}

// 检测卡片
uint8_t RC522_CheckCard(uint8_t *uid) {
    uint8_t status;
    uint8_t tag_type[2];
    
    // 只检测，不选卡，不休眠
    status = RC522_Request(PICC_REQIDL, tag_type);
    if (status == MI_OK) {
        status = RC522_Anticoll(uid);
        // 重要：不调用RC522_SelectTag()和RC522_Halt()
    }
    
    return status;
}

// 寻卡
uint8_t RC522_Request(uint8_t req_mode, uint8_t *tag_type) {
    uint8_t status;
    uint16_t back_bits;
    
    RC522_Write(BitFramingReg, 0x07);
    
    tag_type[0] = req_mode;
    status = RC522_ToCard(PCD_TRANSCEIVE, tag_type, 1, tag_type, &back_bits);
    
    if ((status != MI_OK) || (back_bits != 0x10)) {
        status = MI_ERR;
    }
    
    return status;
}

// 防碰撞
uint8_t RC522_Anticoll(uint8_t *ser_num) {
    uint8_t status;
    uint8_t i;
    uint8_t ser_num_check = 0;
    uint16_t unLen;
    
    RC522_Write(BitFramingReg, 0x00);
    ser_num[0] = PICC_ANTICOLL;
    ser_num[1] = 0x20;
    status = RC522_ToCard(PCD_TRANSCEIVE, ser_num, 2, ser_num, &unLen);
    
    if (status == MI_OK) {
        for (i = 0; i < 4; i++) {
            ser_num_check ^= ser_num[i];
        }
        if (ser_num_check != ser_num[i]) {
            status = MI_ERR;
        }
    }
    
    return status;
}

// 选卡
void RC522_SelectTag(uint8_t *ser_num) {
    uint8_t i;
    uint16_t back_len;
    uint8_t buffer[9];
    
    buffer[0] = PICC_SELECTTAG;
    buffer[1] = 0x70;
    for (i = 0; i < 5; i++) {
        buffer[i + 2] = ser_num[i];
    }
    
    RC522_ToCard(PCD_TRANSCEIVE, buffer, 7, buffer, &back_len);
}

// 休眠
void RC522_Halt(void) {
    uint16_t unLen;
    uint8_t buff[4];
    
    buff[0] = PICC_HALT;
    buff[1] = 0;
    RC522_ToCard(PCD_TRANSCEIVE, buff, 2, buff, &unLen);
}

// 与卡片通信 - 这是最重要的修改！
// 将 void 改为 uint8_t，并确保返回 status
uint8_t RC522_ToCard(uint8_t command, uint8_t *send_data, uint8_t send_len,
                     uint8_t *back_data, uint16_t *back_len) {
    uint8_t status = MI_ERR;
    uint8_t irqEn = 0x00;
    uint8_t waitIRq = 0x00;
    uint8_t lastBits;
    uint8_t n;
    uint16_t i;
    
    switch (command) {
        case PCD_AUTHENT:
            irqEn = 0x12;
            waitIRq = 0x10;
            break;
        case PCD_TRANSCEIVE:
            irqEn = 0x77;
            waitIRq = 0x30;
            break;
        default:
            break;
    }
    
    RC522_Write(ComIEnReg, irqEn | 0x80);
    RC522_ClearBitMask(ComIrqReg, 0x80);
    RC522_SetBitMask(FIFOLevelReg, 0x80);
    
    RC522_Write(CommandReg, PCD_IDLE);
    
    for (i = 0; i < send_len; i++) {
        RC522_Write(FIFODataReg, send_data[i]);
    }
    
    RC522_Write(CommandReg, command);
    
    if (command == PCD_TRANSCEIVE) {
        RC522_SetBitMask(BitFramingReg, 0x80);
    }
    
    i = 2000;
    do {
        n = RC522_Read(ComIrqReg);
        i--;
    } while ((i != 0) && !(n & 0x01) && !(n & waitIRq));
    
    RC522_ClearBitMask(BitFramingReg, 0x80);
    
    if (i != 0) {
        if (!(RC522_Read(ErrorReg) & 0x1B)) {
            status = MI_OK;
            if (n & irqEn & 0x01) {
                status = MI_NOTAGERR;
            }
            
            if (command == PCD_TRANSCEIVE) {
                n = RC522_Read(FIFOLevelReg);
                lastBits = RC522_Read(ControlReg) & 0x07;
                if (lastBits) {
                    *back_len = (n - 1) * 8 + lastBits;
                } else {
                    *back_len = n * 8;
                }
                
                if (n == 0) {
                    n = 1;
                }
                if (n > 16) {
                    n = 16;
                }
                
                for (i = 0; i < n; i++) {
                    back_data[i] = RC522_Read(FIFODataReg);
                }
            }
        } else {
            status = MI_ERR;
        }
    }
    
    RC522_Write(CommandReg, PCD_IDLE);
    
    return status;  // 这是必须添加的一行！
}
										 /* USER CODE BEGIN 4 */
// RC522诊断函数
void RC522_Diagnose(void) {
    // 1. 读取版本号
    uint8_t version = RC522_Read(VersionReg);
    
    // 2. 检查关键寄存器
    uint8_t cmd_reg = RC522_Read(CommandReg);
    uint8_t tx_ctrl = RC522_Read(TxControlReg);
    uint8_t rx_thresh = RC522_Read(RxThresholdReg);
    
    // 可以通过串口输出这些值调试
    // printf("Version: 0x%02X\n", version);
    // printf("CommandReg: 0x%02X\n", cmd_reg);
    // printf("TxControlReg: 0x%02X\n", tx_ctrl);
    // printf("RxThresholdReg: 0x%02X\n", rx_thresh);
    
    // 3. 简单测试：发送REQIDL命令
    uint8_t tag_type[2];
    uint8_t status = RC522_Request(PICC_REQIDL, tag_type);
    
    // printf("RC522_Request status: %d\n", status);
}

// 简单的卡片检测函数（返回0/1）
uint8_t RC522_SimpleCheck(void) {
    uint8_t tag_type[2];
    return (RC522_Request(PICC_REQIDL, tag_type) == MI_OK);
}
/* USER CODE END 4 */

