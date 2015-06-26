// Include the common option parsing interfaces.
include "llvm/Option/OptParser.td"


/////////
// Flags

// DriverOption - The option is a "driver" option, and should not be forwarded
// to other tools.
def DriverOption : OptionFlag;

// LinkerInput - The option is a linker input.
def LinkerInput : OptionFlag;

// NoArgumentUnused - Don't report argument unused warnings for this option; this
// is useful for options like -static or -dynamic which a user may always end up
// passing, even if the platform defaults to (or only supports) that option.
def NoArgumentUnused : OptionFlag;

// Unsupported - The option is unsupported, and the driver will reject command
// lines that use it.
def Unsupported : OptionFlag;

// CoreOption - This is considered a "core" Clang option, available in both
// clang and clang-cl modes.
def CoreOption : OptionFlag;

// NoDriverOption - This option should not be accepted by the driver.
def NoDriverOption : OptionFlag;

/////////
// Groups

// Meta-group for options which are only used for compilation,
// and not linking etc.
def CompileOnly_Group     : OptionGroup<"<CompileOnly group>">;

def Action_Group          : OptionGroup<"<action group>">;



///////////
// Options

def help : Flag<["-", "--"], "help">, Flags<[NoDriverOption]>,
    HelpText<"Display available options">;

def o : JoinedOrSeparate<["-"], "o">, Flags<[RenderAsInput, DriverOption]>, MetaVarName<"<file>">,
    HelpText<"Write output to <file>">;

let Flags = [NoDriverOption] in {

def target_abi : Separate<["-"], "target-abi">,
  HelpText<"Target a particular ABI type">;
def target_cpu : Separate<["-"], "target-cpu">,
  HelpText<"Target a specific cpu type">;
def target_feature : Separate<["-"], "target-feature">,
  HelpText<"Target specific attributes">;
def triple : Separate<["-"], "triple">,
  HelpText<"Specify target triple (e.g. i686-apple-darwin9)">;

}

def triple_EQ : Joined<["-"], "triple=">, Alias<triple>;
def mfpmath : Separate<["-"], "mfpmath">,
  HelpText<"Which unit to use for fp math">;

def target_linker_version : Separate<["-"], "target-linker-version">,
  HelpText<"Target linker version">;
