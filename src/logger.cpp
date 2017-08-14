#include "src/logger.h"
#include <QDir>

Logger::Logger(QObject *parent, QString fileName, QPlainTextEdit *editor) : QObject(parent) {
 m_editor = editor;
 m_showDate = true;

 QDir dir;
 if( !dir.exists( "journal" ) ) dir.mkdir( "journal" );

 if (!fileName.isEmpty())
 {
  file = new QFile;
  file->setFileName(fileName);
  file->open(QIODevice::ReadWrite | QIODevice::Text);
  while(!file->atEnd())
  {
      QString line = file->readLine();
      line.chop(2);
      m_editor->appendPlainText(line);
  }
 }
}

void Logger::write(const QString &value) {
 QString text = value;// + "";
 if (m_showDate)
  text = QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss ") + text;
 QTextStream out(file);
 out.setCodec("UTF-8");
 if (file != 0) {
  out << text;
 }
 if (m_editor != 0)
  m_editor->appendPlainText(text);
}

void Logger::setShowDateTime(bool value) {
 m_showDate = value;
}

Logger::~Logger() {
 if (file != 0)
 file->close();
}
