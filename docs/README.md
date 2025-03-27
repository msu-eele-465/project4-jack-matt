
# Project4
## Main Flowchart
![Alt text](Flowcharts/LCD-MAIN.drawio.svg)

## Circuit Diagram
![Alt text](Project4_CircuitDiagram.drawio.svg)

## Software architecture
```mermaid
graph TB
    subgraph "Main Controller (MSP430FR2355)"
        MC[Main Controller] --> Keypad[Keypad Module]
        MC --> Status[Status LED Module]
        MC --> Heart[Heartbeat LED]
        MC --> I2C_Master[I2C Master Module]
        MC --> Auth[Auth Module]
        
        subgraph "Controller Components"
            Keypad --> KeypadScan[Keypad Scanner]
            Keypad --> KeypadInterrupt[Keypad Interrupt]
            Status --> RGB[RGB LED Control]
            Auth --> UnlockCode[Unlock Code Handler]
            I2C_Master --> I2C_LED_Control[LED Pattern Command]
            I2C_Master --> I2C_LCD_Control[LCD Command]
        end
    end
    
    subgraph "LED Peripheral (MSP430FR2310)"
        LED_MCU[LED Controller] --> LED_I2C[I2C Slave Module]
        LED_MCU --> LED_Status[Status Indicator]
        LED_MCU --> LED_BAR[LED Bar Interface]
        
        subgraph "LED Components"
            LED_I2C --> LED_CMD[Command Parser]
            LED_BAR --> Pattern0[Static Pattern]
            LED_BAR --> Pattern1[Toggle Pattern]
            LED_BAR --> Pattern2[Up Counter]
            LED_BAR --> Pattern3[In and Out]
            LED_BAR --> PatternExtra[Extra Patterns]
            LED_BAR --> TransitionTime[Transition Timer]
        end
    end
    
    subgraph "LCD Peripheral (MSP430FR2310)"
        LCD_MCU[LCD Controller] --> LCD_I2C[I2C Slave Module]
        LCD_MCU --> LCD_Status[Status Indicator]
        LCD_MCU --> LCD_Interface[LCD Interface]
        
        subgraph "LCD Components"
            LCD_I2C --> LCD_CMD[Command Parser]
            LCD_Interface --> CharDisplay[Character Display]
            LCD_Interface --> CursorControl[Cursor Control]
            LCD_Interface --> PatternDisplay[Pattern Name Display]
            LCD_Interface --> KeypressDisplay[Keypress Display]
            LCD_Interface --> ContrastControl[Contrast Control]
        end
    end
    
    %% Communication flow
    I2C_Master -->|I2C Commands| LED_I2C
    I2C_Master -->|I2C Commands| LCD_I2C
    
    %% External inputs
    User[User Input] -->|Button Press| Keypad
    
    %% Class styling
    classDef controller fill:#f9f,stroke:#333,stroke-width:2px
    classDef peripheral fill:#bbf,stroke:#333,stroke-width:2px
    classDef module fill:#dfd,stroke:#333,stroke-width:1px
    
    class MC,LED_MCU,LCD_MCU controller
    class Keypad,Status,Heart,I2C_Master,LED_I2C,LCD_I2C,LED_BAR,LCD_Interface,LED_Status,LCD_Status peripheral
    class KeypadScan,KeypadInterrupt,RGB,UnlockCode,I2C_LED_Control,I2C_LCD_Control,LED_CMD,LCD_CMD,Pattern0,Pattern1,Pattern2,Pattern3,PatternExtra,TransitionTime,CharDisplay,CursorControl,PatternDisplay,KeypressDisplay,ContrastControl module
