# mini-Project in IoT Classs

## Class Description:

Internet of Things (IoT) technology. IoT achitecture. Structure of IoT device. Introduction to sensor and actuator. M2M network and protocol. Gateway and edge computing. Internet connectivity protocols including REST and MQTT. Cloud computing for IoT. Concepts of end-to-end security. IoT device and network management. Case studies of IoT applications in smart factory and smart cities.

## SLEEP DIAGNOSE

มินิโปรเจคนี้เป็นการประยุกต์อุปกรณ์ IoT ขนาดเล็กเข้มาเพื่อวินิฉัยการนอน โดยการวัดค่าสภาพแวดล้อมต่างๆในการนอนหละบ และนำข้อมูลต่างๆเหล่านี้เข้ามาวิเคราะห์ผ่านโมเดลแมชชีนเลินนิ่งเพื่อตรวจสอบสภาวะหยุดหายใจขณะหลับ

![](https://paper-attachments.dropbox.com/s_E070B99CBE1E1D919C276A11170FC796926E829257EA7669DE2940C8EBDCE03B_1636343985266_sleepDinoges1.png)


Actigraphy is a validated method of objectively measuring sleep parameters and average motor activity over a period of days to weeks using a noninvasive accelerometer [1-6]. The accelerometer is housed in a small device that is worn like a wristwatch.

![](https://paper-attachments.dropbox.com/s_E070B99CBE1E1D919C276A11170FC796926E829257EA7669DE2940C8EBDCE03B_1636346311971_image.png)


ปัญหาสุขภาพที่เกี่ยวกับระบบทางเดินหายใจ อาการหายใจผิดปกติ เป็นอันตรายใกล้ตัวที่ไม่ควรมองข้าม เนื่องจากอาการเหล่านี้ บางครั้งอาจไม่ได้บ่งบอกอาการของโรคอย่างชัดเจน เช่น อาการหยุดหายใจขณะหลับ ที่เป็นปัญหาต่อสุขภาพที่เกิดจากการนอนกรน เป็นต้น ซึ่งการป้องกันรักษาหรือการดูแลตนเองจากปัญหาสุขภาพเหล่านี้ อาจมีศัพท์ทางการแพทย์ที่ใช้เรียกหรือใช้ทับศัพท์อยู่หลายคำ ซึ่งหลายท่านอาจไม่ทราบความหมาย เช่น Sleep Apnea คำนี้คืออะไร หาคำตอบได้จากบทความต่อไปนี้

![](https://paper-attachments.dropbox.com/s_E070B99CBE1E1D919C276A11170FC796926E829257EA7669DE2940C8EBDCE03B_1636346845391_image.png)

![](https://paper-attachments.dropbox.com/s_E070B99CBE1E1D919C276A11170FC796926E829257EA7669DE2940C8EBDCE03B_1636346671313_image.png)


ในโปรเจคนี้ใช้การวัดอุณหภมิและความชื้น(BMP280) และวัดการขยับของร่างกาย(MPU6050)จากบอร์ดCucomberRs ของบริษัท Gravitech 

![](https://paper-attachments.dropbox.com/s_E070B99CBE1E1D919C276A11170FC796926E829257EA7669DE2940C8EBDCE03B_1636345783604_image.png)

## โดยการทำงานจะเป็นดังนี้
![](https://paper-attachments.dropbox.com/s_E070B99CBE1E1D919C276A11170FC796926E829257EA7669DE2940C8EBDCE03B_1636347013173_image.png)

![](https://paper-attachments.dropbox.com/s_E070B99CBE1E1D919C276A11170FC796926E829257EA7669DE2940C8EBDCE03B_1636347789388_image.png)



## ขั้นตอนที่1: เก็บข้อมูลสำหรับวิเคราะห์

ในขั้นตอนนี้จะแบ่งออกเป็นสองส่วนคือ ส่วนของอุปกรณ์(device) และส่วนของระบบเก็บข้อมูล(server)
**Device:**จะเขียนโปรแกรมเพื่อทำการเก็บข้อมูลจากการขยับของหน้าอกโดยใช้เซนเซอร์ความเร่ง(MPU6050)ที่บอร์ด[CucumberRs](https://www.gravitechthai.com/product-detail.php?WP=rUEjoz1yq3EZoz1iM2A0G2zDrYyj4T1zqlMZG22DM7y04TyjrPMjZT1Cq5OZhJ3tM2E0nJyyrUEjLJ1wq2WZqJ1mMlM0ZTyCrWOjhJ3tq2EZnJ1yM3E0LJywrPMjZT1Cq5OZhJ3tM2E0nJyarPMjA204qmMZAT1CM5O0hJatrTZo7o3Q)ติดมาให้กับตัวอุปกรณ์ โดยทำการเก็บข้อมูลเป็นเซ็ตเซ็ตละ5วินาทีและทำการส่งไปยังระบบเก็บข้อมูลผ่านwifiโดยที่มีsampling rate 200 (1ชุดข้อมูลมีข้อมูลภายใน{x,y,z}200ค่า) หลังจากนั้นจะส่ง POST method ในรูปแบบ json package ไปยัง server
**Server:**คอยเปิดช่องทางการเชื่อมต่อระหว่างอุปกรณ์เมื่อมีอุปกรณ์มาเชื่อมต่อจะตอบกลับสถานะความพร้อม(response:1)ไปยังอุปกรณ์เพื่อป้องกันการผิดพลาดในการรับ-ส่งข้อมูล หลังจากได้ข้อมูลจากอุปกรณ์มาแล้วจะทำการบันทึกลงบนอุปกรณ์ในรูปแบบ .csv ไฟล์

![](https://paper-attachments.dropbox.com/s_E070B99CBE1E1D919C276A11170FC796926E829257EA7669DE2940C8EBDCE03B_1639805680944_image.png)


ในการเก็บข้อมูลเพื่อวิเคราะห์เบื้องต้นโดยใช้กาวติดอุปกรณ์บริเวณหน้าอกของผู้ทดลอง และทำการเก็บข้อมูล 20set โดยที่จะแบ่งออกเป็นการหายใจแบบปกติ และการเกิดภาวะหยุดกายใจ(apnea)

![](https://paper-attachments.dropbox.com/s_E070B99CBE1E1D919C276A11170FC796926E829257EA7669DE2940C8EBDCE03B_1639804390752_image.png)


ข้อมูลที่ได้จะแยกออกเป็นสองส่วนคือส่วนของการหายใจแบบปกติและเกิดภาวะหยุดหายใจ

![](https://paper-attachments.dropbox.com/s_E070B99CBE1E1D919C276A11170FC796926E829257EA7669DE2940C8EBDCE03B_1639806654902_image.png)



## ขั้นตอนที่2:วิเคราะห์และสกัดข้อมูล

สกัดข้อมูล(feature extraction)
[jupyter notebook](https://paper.dropbox.com/doc/--BYTjsXKxtYK0ue9E9_96fFocAg-MnYI7z1xBrOqh003y2Snk)

![](https://paper-attachments.dropbox.com/s_E070B99CBE1E1D919C276A11170FC796926E829257EA7669DE2940C8EBDCE03B_1639807185400_image.png)


เทรนโมเดล
[jupyter notebook](https://paper.dropbox.com/doc/--BYTjsXKxtYK0ue9E9_96fFocAg-MnYI7z1xBrOqh003y2Snk)

![](https://paper-attachments.dropbox.com/s_E070B99CBE1E1D919C276A11170FC796926E829257EA7669DE2940C8EBDCE03B_1639844234803_image.png)


แปลงโมเดล(convert to C)
[jupyter notebook](https://paper.dropbox.com/doc/--BYTjsXKxtYK0ue9E9_96fFocAg-MnYI7z1xBrOqh003y2Snk)

![](https://paper-attachments.dropbox.com/s_E070B99CBE1E1D919C276A11170FC796926E829257EA7669DE2940C8EBDCE03B_1639844406674_image.png)



**ขั้นตอนที่3: Deploy model**
หลังจากที่เราได้ [C file](http://www.com) มาแล้วนำโมเดลเที่เรามารวมไว้ในโฟลเดอร์สำหรับdeployลงcontroller  

![](https://paper-attachments.dropbox.com/s_E070B99CBE1E1D919C276A11170FC796926E829257EA7669DE2940C8EBDCE03B_1639880793839_image.png)

![](https://paper-attachments.dropbox.com/s_E070B99CBE1E1D919C276A11170FC796926E829257EA7669DE2940C8EBDCE03B_1639903410468_image.png)



## ขั้นตอนที่4:ประยุกต์ใช้ในLINE Application

            โดยในส่วนนี้เลือการทำงานผ่านDialogFlowเพื่อให้ง่ายต่อการสร้างระบบโต้ตอบกับผู้ใช้งนผ่านข้อความ ดพื่อให้ผู้ที่สนใจนำประประยุกต์ใช้สามารถนำแนวทางนี้ไปพัฒนาได้โดยไม่ต้องอาศัยความรู้ในการโปรแกรมมิงมากนักและสามารถแต่งตั้งผู้อื่นมาช่วยพัฒนาข้อความโต้ตอบกับผู้ใช้งานเพื่อให้เกิดความรวดเร็วในการพัฒนาหรือประยุกต์ใช้ในงานต่างๆ 
            นอกเหนือจากข้อความที่เป็นตัวอักษรแล้วยังสามารถการโต้ตอบหรือจัดเก็บรูปถาพที่ผู้ใช้งานโต้ตอบในแชทโดยการใช้webHook จาก dialogFlow ในการเก็บข้อมูลคอนเท้นต่างๆจากภายในแชทย์และยังสามารถเขียนโปรแกรมเพิ่มเติมเพื่อโต้ตอบและวิเคราห์ข้อมูลโดยรับอินพุตจากในแชท ไม่ว่าจะเป็นการวิเคราะห์รูปภาพ การคำณวนพารามิเตอร์ต่างๆ และตอบกลับไปยังผู้ใช้งาน

![](https://paper-attachments.dropbox.com/s_E070B99CBE1E1D919C276A11170FC796926E829257EA7669DE2940C8EBDCE03B_1639904001087_image.png)


เริ่มจากเข้าเว็บ https://developers.line.biz/ เพื่อทำการสร้าง ไลน์บอทและกดเลือก Start using Messaging API

![](https://paper-attachments.dropbox.com/s_E070B99CBE1E1D919C276A11170FC796926E829257EA7669DE2940C8EBDCE03B_1639904711104_image.png)


หลังจากนั้นทำการใส่ค่าต่างๆให้เรียบร้อย(แค่คร่าวๆเดี๋ยวค่อยมาทำต่อ) ต่อมาให้เข้าไปที่
https://dialogflow.cloud.google.com/

![](https://paper-attachments.dropbox.com/s_E070B99CBE1E1D919C276A11170FC796926E829257EA7669DE2940C8EBDCE03B_1639905150663_image.png)


ทำสามขั้นตอนนี้เพื่อเป็นการสร้างintent(และหลังจากนี้ ขก.ทำตามนี้ [Link](https://siriphonnot.medium.com/%E0%B8%AA%E0%B8%A3%E0%B9%89%E0%B8%B2%E0%B8%87-line-chatbot-%E0%B8%87%E0%B9%88%E0%B8%B2%E0%B8%A2%E0%B9%86-%E0%B8%94%E0%B9%89%E0%B8%A7%E0%B8%A2-dialogflow-207e86487651))
เมื่อเชื่อมต่อไลน์กับdialogDialogflowเรียบร้อยแล้ว เราจะมาเชื่อมต่อDialogFlowกับเซอเวอร์flask ของเรากัน
เข้าไปที่ fulfillment และกด ENABLE หน้าเว็บฮุก และใส่ลิงค์ที่สามารถเข้าถึงServerเราเข้าไป

![](https://paper-attachments.dropbox.com/s_E070B99CBE1E1D919C276A11170FC796926E829257EA7669DE2940C8EBDCE03B_1639905441276_image.png)

![](https://paper-attachments.dropbox.com/s_E070B99CBE1E1D919C276A11170FC796926E829257EA7669DE2940C8EBDCE03B_1639905644738_image.png)


ภาในไฟล์Serverจะประกอบไปด้วยตัวอย่างการใช้งานดังนี้

![](https://paper-attachments.dropbox.com/s_E070B99CBE1E1D919C276A11170FC796926E829257EA7669DE2940C8EBDCE03B_1639905939736_image.png)



# ทดสอบการใช้งาน

ทดสอบการใช้งานของไลน์บอท

![](https://paper-attachments.dropbox.com/s_E070B99CBE1E1D919C276A11170FC796926E829257EA7669DE2940C8EBDCE03B_1639910669394_image.png)


ในการทดสอบจะทำการเทสฟังก์ชั่นต่างๆของไลน์บอทโดยมีการสร้างrich menuเพื่อให้ง่ายต่อการใช้งาน

![](https://paper-attachments.dropbox.com/s_E070B99CBE1E1D919C276A11170FC796926E829257EA7669DE2940C8EBDCE03B_1639910888124_image.png)


