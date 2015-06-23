// Define the diagnostic severities.
class Severity<string N> { string Name = N; }
def SEV_Ignored : Severity<"Ignored">;
def SEV_Remark  : Severity<"Remark">;
def SEV_Warning : Severity<"Warning">;
def SEV_Error   : Severity<"Error">;
def SEV_Fatal   : Severity<"Fatal">;

// Define the diagnostic classes.
class DiagClass;
def CLASS_NOTE      : DiagClass;
def CLASS_REMARK    : DiagClass;
def CLASS_WARNING   : DiagClass;
def CLASS_EXTENSION : DiagClass;
def CLASS_ERROR     : DiagClass;

// Diagnostic Categories.  These can be applied to groups or individual
// diagnostics to specify a category.
class DiagCategory<string Name> {
      string CategoryName = Name;
}

// Diagnostic Groups.
class DiagGroup<string Name, list<DiagGroup> subgroups = []> {
      string GroupName = Name;
      list<DiagGroup> SubGroups = subgroups;
      string CategoryName = "";
}
class InGroup<DiagGroup G> { DiagGroup Group = G; }

// This defines all of the named diagnostic categories.
include "DiagnosticCategories.td"

// This defines all of the named diagnostic groups.
include "DiagnosticGroups.td"

// All diagnostics emitted by the compiler are an indirect subclass of this.
class Diagnostic<string text, DiagClass dc, Severity defaultmapping> {
      /// Component is specified by the file with a big let directive.
      string         Component = ?;
      string         CategoryName = "";
      string         Text = text;
      Severity       DefaultSeverity = defaultmapping;
      DiagClass      Class = dc;
      DiagGroup      Group;
      bit            WarningNoWerror = 0;
}

class Error<string str>     : Diagnostic<str, CLASS_ERROR,      SEV_Error>;
class Warning<string str>   : Diagnostic<str, CLASS_WARNING,    SEV_Warning>;
class Remark<string str>    : Diagnostic<str, CLASS_REMARK,     SEV_Ignored>;
class Extension<string str> : Diagnostic<str, CLASS_EXTENSION,  SEV_Ignored>;
class ExtWarn<string str>   : Diagnostic<str, CLASS_EXTENSION,  SEV_Warning>;
class Note<string str>      : Diagnostic<str, CLASS_NOTE,       SEV_Fatal/*ignored*/>;

class DefaultIgnore         { Severity DefaultSeverity = SEV_Ignored; }
class DefaultWarn           { Severity DefaultSeverity = SEV_Warning; }
class DefaultError          { Severity DefaultSeverity = SEV_Error; }
class DefaultFatal          { Severity DefaultSeverity = SEV_Fatal; }
class DefaultRemark         { Severity DefaultSeverity = SEV_Remark; }
class DefaultWarnNoWerror   { bit WarningNoWerror = 1; }

// Definitions for Diagnostics.
include "DiagnosticCommonKinds.td"
include "DiagnosticDriverKinds.td"
include "DiagnosticSemaKinds.td"
