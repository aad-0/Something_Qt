#include "csvwriter.h"
#include <QDateTime>
#include <QDebug>

// Constructor: Initializes the file path and opens the file for writing.
// If the file is new or empty, it writes a header row.
CsvWriter::CsvWriter(const QString& filePath, QObject *parent)
    : QObject(parent), _mFilePath(filePath), _mIsFileOpen(false)
{
    // Add a timestamp to the filename to make it unique each run
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    _mFilePath = QString("accel_data_%1.csv").arg(timestamp); // Construct filename

    _mFile.setFileName(_mFilePath);
    if (_mFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) 
    {
        _mTextStream.setDevice(&_mFile);
        _mIsFileOpen = true;
        qDebug() << "Opened CSV file for writing:" << _mFilePath;
        // Write header if file is new/empty
        if (_mFile.size() == 0) 
        {
            _mTextStream << "Timestamp,Tick,Encoder,X,Y,Z\n";
            _mTextStream.flush(); // Ensure header is written immediately
            qDebug() << "Wrote CSV header.";
        }
    } else {
        qWarning() << "Error opening file for writing:" << _mFilePath << "Error:" << _mFile.errorString();
        _mIsFileOpen = false;
    }
     // Register the ComDefImu_TypeDef type if CsvWriter is in a different thread 
     // and receives this type via queued connections (already done in AccelDevice)
    qRegisterMetaType<ComDefImu_TypeDef>("ComDefImu_TypeDef");
}

// Destructor: Ensures the file is closed when the CsvWriter object is destroyed.
CsvWriter::~CsvWriter()
{
    if (_mFile.isOpen()) {
        _mTextStream.flush(); // Ensure all buffered data is written
        _mFile.close();
        qDebug() << "Closed CSV file:" << _mFilePath;
    }
}

// writeImuData: Slot that receives IMU data and writes it as a new row in the CSV file.
void CsvWriter::writeImuData(ComDefImu_TypeDef data)
{
    if (!_mIsFileOpen) {
        // Optional: Could try reopening the file here, or just skip writing
        qWarning() << "CSV file is not open, cannot write data.";
        return; 
    }

    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODateWithMs);

    // Use QTextStream for easy formatting, ensuring locale doesn't affect decimal points
    _mTextStream.setRealNumberPrecision(6); // Set precision for float values
    _mTextStream.setRealNumberNotation(QTextStream::FixedNotation);

    _mTextStream << timestamp << "," 
                 << data.Tick << "," 
                 << data.Encoder << "," 
                 << data.fX << "," 
                 << data.fY << "," 
                 << data.fZ << "\n";

    // Optionally flush periodically instead of every write for better performance
    // _mTextStream.flush(); 

    // Example: Log to debug output as well
    // qDebug() << "Logged IMU to CSV:" << timestamp << data.Tick << data.Encoder << data.fX << data.fY << data.fZ;
} 