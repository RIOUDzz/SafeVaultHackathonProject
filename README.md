# SafeVault - Secure Message Storage

[English](#english) | [العربية](#arabic)

# English

A secure message storage system that allows users to register, login, and store encrypted messages using MySQL database and Caesar cipher encryption.

# Arabic - العربية

نظام تخزين رسائل آمن يسمح للمستخدمين بالتسجيل وتسجيل الدخول وتخزين الرسائل المشفرة باستخدام قاعدة بيانات MySQL وتشفير قيصر.

## Features

- Secure user registration and authentication
- Password requirements (uppercase, lowercase, numbers, special characters)
- Message encryption using Caesar cipher (shift by 3)
- MySQL database storage for users and messages
- Simple command-line interface
- Secure message viewing and storage
- Session management with logout functionality


## المميزات

- تسجيل وتوثيق آمن للمستخدمين
- متطلبات كلمة المرور (أحرف كبيرة، أحرف صغيرة، أرقام، رموز خاصة)
- تشفير الرسائل باستخدام شفرة قيصر (إزاحة بمقدار 3)
- تخزين في قاعدة بيانات MySQL للمستخدمين والرسائل
- واجهة سطر أوامر بسيطة
- عرض وتخزين آمن للرسائل
- إدارة الجلسات مع إمكانية تسجيل الخروج

## Project Structure | هيكل المشروع

```
├── main.c              # Main program and user interface | البرنامج الرئيسي وواجهة المستخدم
├── authentication.c    # User authentication logic | منطق المصادقة
├── authentication.h    # Authentication headers | رؤوس المصادقة
├── database.c         # Database operations | عمليات قاعدة البيانات
├── database.h         # Database headers | رؤوس قاعدة البيانات
├── db_config.h        # Database configuration | إعدادات قاعدة البيانات
├── encryption.c       # Message encryption/decryption | تشفير/فك تشفير الرسائل
├── encryption.h       # Encryption headers | رؤوس التشفير
├── storage.c          # Message storage operations | عمليات تخزين الرسائل
├── storage.h          # Storage headers | رؤوس التخزين
└── README.md          # Documentation | التوثيق
```


## Database Structure | هيكل قاعدة البيانات

### Users Table | جدول المستخدمين
```sql
CREATE TABLE users (
    id INT AUTO_INCREMENT PRIMARY KEY,
    username VARCHAR(50) UNIQUE NOT NULL,
    hashed_password BINARY(32) NOT NULL,
    salt BINARY(16) NOT NULL,
    failed_attempts INT DEFAULT 0,
    last_attempt TIMESTAMP NULL,
    is_locked BOOLEAN DEFAULT FALSE
);
```

### Messages Table | جدول الرسائل
```sql
CREATE TABLE messages (
    id INT AUTO_INCREMENT PRIMARY KEY,
    user_id INT NOT NULL,
    encrypted_message TEXT NOT NULL,
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (user_id) REFERENCES users(id)
);
```

## Security Features | ميزات الأمان

### Password Security | أمان كلمة المرور
- Passwords must contain uppercase, lowercase, numbers, and special characters
- Passwords are hashed using a secure algorithm
- Salt is added to prevent rainbow table attacks
- Account lockout after multiple failed attempts

### Message Encryption | تشفير الرسائل
```c
char* caesar_encrypt(const char* text) {
    if (!text) return NULL;
    
    int len = strlen(text);
    char* result = (char*)malloc(len + 1);
    
    for (int i = 0; i < len; i++) {
        if (isalpha(text[i])) {
            char base = isupper(text[i]) ? 'A' : 'a';
            result[i] = (text[i] - base + SHIFT_KEY) % 26 + base;
        } else {
            result[i] = text[i];
        }
    }
    result[len] = '\0';
    return result;
}
```

## User Flow | تدفق المستخدم

### Main Menu | القائمة الرئيسية
1. Register new user | تسجيل مستخدم جديد
2. Login existing user | تسجيل دخول مستخدم موجود
3. Exit program | الخروج من البرنامج

### Registration | التسجيل
1. Enter username | إدخال اسم المستخدم
2. Enter password (must meet requirements) | إدخال كلمة المرور (يجب أن تستوفي المتطلبات)
3. System creates account in database | النظام ينشئ الحساب في قاعدة البيانات

### Login | تسجيل الدخول
1. Enter username | إدخال اسم المستخدم
2. Enter password | إدخال كلمة المرور
3. System verifies credentials | النظام يتحقق من البيانات

### User Menu | قائمة المستخدم
1. Write new message | كتابة رسالة جديدة
2. View messages | عرض الرسائل
3. Logout | تسجيل الخروج
4. Exit | الخروج

## Installation & Setup | التثبيت والإعداد

### Requirements | المتطلبات
- GCC compiler | مترجم GCC
- MySQL server (XAMPP) | خادم MySQL (XAMPP)
- MySQL development libraries | مكتبات تطوير MySQL

### Database Setup | إعداد قاعدة البيانات
1. Start XAMPP MySQL server | تشغيل خادم MySQL من XAMPP
```bash
sudo /opt/lampp/lampp startmysql
```

2. Create database | إنشاء قاعدة البيانات
```bash
/opt/lampp/bin/mysql -u root -e "CREATE DATABASE IF NOT EXISTS safevault;"
```

### Compilation | التجميع
```bash
gcc -o safevault main.c database.c authentication.c encryption.c storage.c -lmysqlclient -lcrypto
```

### Running | التشغيل
```bash
./safevault
```

## Error Handling | معالجة الأخطاء

### Input Validation | التحقق من المدخلات
- Username validation | التحقق من اسم المستخدم
- Password complexity requirements | متطلبات تعقيد كلمة المرور
- Buffer overflow protection | الحماية من تجاوز المخزن المؤقت
- SQL injection prevention | منع حقن SQL

### Database Operations | عمليات قاعدة البيانات
- Connection error handling | معالجة أخطاء الاتصال
- Query error handling | معالجة أخطاء الاستعلام
- Transaction management | إدارة المعاملات

### Memory Management | إدارة الذاكرة
- Proper memory allocation | تخصيص الذاكرة المناسب
- Memory deallocation | تحرير الذاكرة
- NULL pointer checks | فحص المؤشرات الفارغة

## Security Features | ميزات الأمان

### Authentication | المصادقة
- Secure password storage | تخزين آمن لكلمات المرور
- Account lockout | قفل الحساب
- Session management | إدارة الجلسات

### Data Protection | حماية البيانات
- Message encryption | تشفير الرسائل
- User data isolation | عزل بيانات المستخدم
- Secure database connections | اتصالات آمنة بقاعدة البيانات

## Current Limitations | القيود الحالية

### Security | الأمان
- Caesar cipher is not cryptographically secure | شفرة قيصر ليست آمنة تشفيرياً
- No end-to-end encryption | لا يوجد تشفير من طرف إلى طرف
- Basic session management | إدارة جلسات أساسية

### Features | المميزات
- No message editing | لا يمكن تعديل الرسائل
- No message deletion | لا يمكن حذف الرسائل
- No password recovery | لا توجد استعادة كلمة المرور
- Text-based interface only | واجهة نصية فقط

## Future Improvements | التحسينات المستقبلية

### Security | الأمان
- Implement AES encryption | تنفيذ تشفير AES
- Add two-factor authentication | إضافة المصادقة الثنائية
- Implement secure key exchange | تنفيذ تبادل المفاتيح الآمن

### Features | المميزات
- GUI using GTK/Qt | واجهة رسومية باستخدام GTK/Qt
- Message editing and deletion | تعديل وحذف الرسائل
- Password recovery system | نظام استعادة كلمة المرور
- Binary file encryption | تشفير الملفات الثنائية

## Contributing | المساهمة

This project was created for educational purposes. Contributions are welcome!
تم إنشاء هذا المشروع لأغراض تعليمية. المساهمات مرحب بها!

## License | الترخيص

MIT License
رخصة MIT
   ```

2. تشغيل البرنامج:
   ```bash
   ./safevault
   ```

2. Run the program:
   ```bash
   ./safevault
   ```

3. اتبع التعليمات على الشاشة للقيام بما يلي:
   - تسجيل حساب جديد
   - تسجيل الدخول إلى حسابك
   - كتابة وعرض الرسائل
   - تسجيل الخروج عند الانتهاء

3. Follow the on-screen instructions to:
   - Register a new account
   - Login to your account
   - Write and view messages
   - Logout when finished

## حل المشاكل

1. **مشاكل تسجيل الدخول**
   - تحقق من اسم المستخدم وكلمة المرور
   - تأكد من تسجيل المستخدم
   - تأكد من استخدام الحروف الصحيحة (الحالة العلوية والسفلية)

1. **Login Issues**
   - Verify username and password
   - Check if user is registered
   - Ensure correct case for username

2. **مشاكل الرسائل**
   - تحقق من طول الرسالة
   - تحقق من مفتاح التشفير
   - تحقق من صلاحيات الملف

2. **Message Issues**
   - Check message length
   - Verify encryption key
   - Check file permissions

3. **مشاكل الملفات**
   - تحقق من صلاحيات الملف
   - تأكد من وجود الملف
   - تحقق من مساحة القرص

3. **File Issues**
   - Check file permissions
   - Verify file existence
   - Check disk space

## المساهمة

1. قم بنسخ المستودع
2. قم بإنشاء فرع جديد
3. قم بإضافة التغييرات
4. قم بنشر التغييرات إلى الفرع
5. قم بإنشاء طلب سحب

## Contributing

1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Push to the branch
5. Create a Pull Request

## الترخيص

هذا المشروع مرخص بموجب ترخيص MIT - راجع ملف الترخيص للتفاصيل

## License

This project is licensed under the MIT License - see the LICENSE file for details