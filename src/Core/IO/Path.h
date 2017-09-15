#pragma once

namespace Framework {

class String;

	namespace Path {

String GetDirectoryName(const String &path);
String GetFileName(const String &path);
String GetFileExtension(const String &path);

	} // namespace Path

} // namespace Framework
