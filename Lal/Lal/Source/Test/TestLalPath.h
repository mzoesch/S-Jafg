// Copyright mzoesch. All rights reserved.

#pragma once

TEST_CASE(EmptyPathOperations, "Lal.System")
{
    LPath Path;
    LBigPath BigPath;
    LSmallPath<25> SmallPath;
    LPathView PathView;
    LMutablePathView MutablePathView;

    QUICK_CHECK_EQUALS(Path.GetRuneCount(), 0ul)
    QUICK_CHECK_EQUALS(BigPath.GetRuneCount(), 0ul)
    QUICK_CHECK_EQUALS(SmallPath.GetRuneCount(), 0ul)
    QUICK_CHECK_EQUALS(PathView.GetRuneCount(), 0ul)
    QUICK_CHECK_EQUALS(MutablePathView.GetRuneCount(), 0ul)

    QUICK_CHECK_EQUALS(Path.GetCharacterCount(), 0ul)
    QUICK_CHECK_EQUALS(BigPath.GetCharacterCount(), 0ul)
    QUICK_CHECK_EQUALS(SmallPath.GetCharacterCount(), 0ul)
    QUICK_CHECK_EQUALS(PathView.GetCharacterCount(), 0ul)
    QUICK_CHECK_EQUALS(MutablePathView.GetCharacterCount(), 0ul)

    QUICK_CHECK_EQUALS(Path.GetAllocatedByteSize(), 8ul)
    QUICK_CHECK_EQUALS(BigPath.GetAllocatedByteSize(), 0ul)
    QUICK_CHECK_EQUALS(SmallPath.GetAllocatedByteSize(), 25ul)
    QUICK_CHECK_EQUALS(PathView.GetAllocatedByteSize(), 0ul)
    QUICK_CHECK_EQUALS(MutablePathView.GetAllocatedByteSize(), 0ul)

    return;
}

TEST_CASE(PathAppendOperations, "Lal.System")
{
    LPath Path { 'A' };

    QUICK_CHECK_EQUALS(Path, 'A')
    QUICK_CHECK_EQUALS(Path, "A")

    Path /= 'B';
    QUICK_CHECK_EQUALS(Path, "A/B")
    Path /= "C";
    QUICK_CHECK_EQUALS(Path, "A/B/C")
    Path /= "/D";
    QUICK_CHECK_EQUALS(Path, "A/B/C/D")
    Path.Append('/');
    QUICK_CHECK_EQUALS(Path, "A/B/C/D/")
    Path /= "E";
    QUICK_CHECK_EQUALS(Path, "A/B/C/D/E")
    Path.Append('/');
    QUICK_CHECK_EQUALS(Path, "A/B/C/D/E/")
    Path /= "/F";
    QUICK_CHECK_EQUALS(Path, "A/B/C/D/E/F")

    return;
}

TEST_CASE(BasicPathOperations, "Lal.System")
{
    std::filesystem::path A;
    LPath B;

    QUICK_CHECK_FALSE(A.has_relative_path())
    QUICK_CHECK_FALSE(B.IsRelativeValid())
    QUICK_CHECK_EQUALS(A.has_relative_path(), B.IsRelativeValid())

    QUICK_CHECK_EQUALS(A.relative_path(), "")
    QUICK_CHECK_EQUALS(B.GetRelativePath(), "")

    A.assign("A");
    B.Assign('A');
    QUICK_CHECK_TRUE(A.has_relative_path())
    QUICK_CHECK_TRUE(B.IsRelativeValid())
    QUICK_CHECK_EQUALS(A.has_relative_path(), B.IsRelativeValid())

    QUICK_CHECK_EQUALS(A.relative_path(), "A")
    QUICK_CHECK_EQUALS(B.GetRelativePath(), "A")

    A.assign("/");
    B.Assign('/');
    QUICK_CHECK_FALSE(A.has_relative_path())
    QUICK_CHECK_FALSE(B.IsRelativeValid())
    QUICK_CHECK_EQUALS(A.has_relative_path(), B.IsRelativeValid())

    QUICK_CHECK_EQUALS(A.relative_path(), "")
    QUICK_CHECK_EQUALS(B.GetRelativePath(), "")

    A.assign("/A");
    B.Assign("/A");
    QUICK_CHECK_TRUE(A.has_relative_path())
    QUICK_CHECK_TRUE(B.IsRelativeValid())
    QUICK_CHECK_EQUALS(A.has_relative_path(), B.IsRelativeValid())

    QUICK_CHECK_EQUALS(A.relative_path(), "A")
    QUICK_CHECK_EQUALS(B.GetRelativePath(), "A")

    A.assign("A/b.p");
    B.Assign("A/b.p");
    QUICK_CHECK_TRUE(A.has_extension())
    QUICK_CHECK_TRUE(B.IsExtensionValid())
    QUICK_CHECK_EQUALS(A.has_extension(), B.IsExtensionValid())

    QUICK_CHECK_EQUALS(A.extension(), ".p")
    QUICK_CHECK_EQUALS(B.GetExtension(), ".p")

    QUICK_CHECK_EQUALS(A.replace_extension(), "A/b")
    QUICK_CHECK_EQUALS(B.RemoveExtension(), "A/b")

    A.assign("A/b.p");
    B.Assign("A/b.p");
    QUICK_CHECK_TRUE(A.has_extension())
    QUICK_CHECK_TRUE(B.IsExtensionValid())
    QUICK_CHECK_EQUALS(A.has_extension(), B.IsExtensionValid())

    A.replace_extension(".huhu");
    B.ReplaceExtension(".huhu");
    QUICK_CHECK_TRUE(A.has_extension())
    QUICK_CHECK_TRUE(B.IsExtensionValid())
    QUICK_CHECK_EQUALS(A.has_extension(), B.IsExtensionValid())
    QUICK_CHECK_EQUALS(A.extension(), ".huhu")
    QUICK_CHECK_EQUALS(B.GetExtension(), ".huhu")

    A.assign("A/b.p");
    B.Assign("A/b.p");
    QUICK_CHECK_TRUE(A.has_extension())
    QUICK_CHECK_TRUE(B.IsExtensionValid())
    QUICK_CHECK_EQUALS(A.has_extension(), B.IsExtensionValid())

    A.replace_extension("huhu");
    B.ReplaceExtension("huhu");
    QUICK_CHECK_TRUE(A.has_extension())
    QUICK_CHECK_TRUE(B.IsExtensionValid())
    QUICK_CHECK_EQUALS(A.has_extension(), B.IsExtensionValid())
    QUICK_CHECK_EQUALS(A, "A/b.huhu")
    QUICK_CHECK_EQUALS(B, "A/b.huhu")
    QUICK_CHECK_EQUALS(A.extension(), ".huhu")
    QUICK_CHECK_EQUALS(B.GetExtension(), ".huhu")

    A.assign("A/b.");
    B.Assign("A/b.");
    QUICK_CHECK_TRUE(A.has_extension())
    QUICK_CHECK_TRUE(B.IsExtensionValid())
    QUICK_CHECK_EQUALS(A.has_extension(), B.IsExtensionValid())

    QUICK_CHECK_EQUALS(A.extension(), ".")
    QUICK_CHECK_EQUALS(B.GetExtension(), ".")

    A.replace_extension("huhu");
    B.ReplaceExtension("huhu");
    QUICK_CHECK_TRUE(A.has_extension())
    QUICK_CHECK_TRUE(B.IsExtensionValid())
    QUICK_CHECK_EQUALS(A.has_extension(), B.IsExtensionValid())
    QUICK_CHECK_EQUALS(A, "A/b.huhu")
    QUICK_CHECK_EQUALS(B, "A/b.huhu")
    QUICK_CHECK_EQUALS(A.extension(), ".huhu")
    QUICK_CHECK_EQUALS(B.GetExtension(), ".huhu")

    A.assign("A/.b");
    B.Assign("A/.b");
    QUICK_CHECK_FALSE(A.has_extension())
    QUICK_CHECK_FALSE(B.IsExtensionValid())
    QUICK_CHECK_EQUALS(A.has_extension(), B.IsExtensionValid())

    QUICK_CHECK_EQUALS(A.extension(), "")
    QUICK_CHECK_EQUALS(B.GetExtension(), "")

    A.assign(".");
    B.Assign('.');
    QUICK_CHECK_FALSE(A.has_extension())
    QUICK_CHECK_FALSE(B.IsExtensionValid())
    QUICK_CHECK_EQUALS(A.has_extension(), B.IsExtensionValid())

    QUICK_CHECK_EQUALS(A.extension(), "")
    QUICK_CHECK_EQUALS(B.GetExtension(), "")

    A.assign("A/.b.d/a");
    B.Assign("A/.b.d/a");
    QUICK_CHECK_FALSE(A.has_extension())
    QUICK_CHECK_FALSE(B.IsExtensionValid())
    QUICK_CHECK_EQUALS(A.has_extension(), B.IsExtensionValid())

    QUICK_CHECK_EQUALS(A.extension(), "")
    QUICK_CHECK_EQUALS(B.GetExtension(), "")

    A.replace_extension(".X");
    B.ReplaceExtension(".X");
    QUICK_CHECK_TRUE(A.has_extension())
    QUICK_CHECK_TRUE(B.IsExtensionValid())

    A.assign("");
    B.Assign("");
    QUICK_CHECK_FALSE(A.has_filename());
    QUICK_CHECK_FALSE(B.IsFilenameValid());
    QUICK_CHECK_EQUALS(A.has_filename(), B.IsFilenameValid());
    QUICK_CHECK_EQUALS(A.filename(), "")
    QUICK_CHECK_EQUALS(B.GetFilename(), "");

    A.assign("A");
    B.Assign("A");
    QUICK_CHECK_TRUE(A.has_filename());
    QUICK_CHECK_TRUE(B.IsFilenameValid());
    QUICK_CHECK_EQUALS(A.has_filename(), B.IsFilenameValid());
    QUICK_CHECK_EQUALS(A.filename(), "A")
    QUICK_CHECK_EQUALS(B.GetFilename(), "A");

    A.assign("A/B");
    B.Assign("A/B");
    QUICK_CHECK_TRUE(A.has_filename());
    QUICK_CHECK_TRUE(B.IsFilenameValid());
    QUICK_CHECK_EQUALS(A.has_filename(), B.IsFilenameValid());
    QUICK_CHECK_EQUALS(A.filename(), "B")
    QUICK_CHECK_EQUALS(B.GetFilename(), "B");

    A.assign("A/B/");
    B.Assign("A/B/");
    QUICK_CHECK_FALSE(A.has_filename());
    QUICK_CHECK_FALSE(B.IsFilenameValid());
    QUICK_CHECK_EQUALS(A.has_filename(), B.IsFilenameValid());
    QUICK_CHECK_EQUALS(A.filename(), "")
    QUICK_CHECK_EQUALS(B.GetFilename(), "");

    A.assign("A");
    A.remove_filename();
    B.Assign("A");
    B.RemoveFilename();
    QUICK_CHECK_FALSE(A.has_filename());
    QUICK_CHECK_FALSE(B.IsFilenameValid());
    QUICK_CHECK_EQUALS(A.has_filename(), B.IsFilenameValid());
    QUICK_CHECK_EQUALS(A.filename(), "")
    QUICK_CHECK_EQUALS(B.GetFilename(), "");

    A.assign("A/B");
    B.Assign("A/B");
    QUICK_CHECK_TRUE(A.has_filename());
    QUICK_CHECK_TRUE(B.IsFilenameValid());
    QUICK_CHECK_EQUALS(A.has_filename(), B.IsFilenameValid());
    QUICK_CHECK_EQUALS(A.filename(), "B")
    QUICK_CHECK_EQUALS(B.GetFilename(), "B");
    A.remove_filename();
    B.RemoveFilename();
    QUICK_CHECK_EQUALS(A, "A/")
    QUICK_CHECK_EQUALS(B, "A/")
    QUICK_CHECK_FALSE(A.has_filename());
    QUICK_CHECK_FALSE(B.IsFilenameValid());
    QUICK_CHECK_EQUALS(A.has_filename(), B.IsFilenameValid());
    QUICK_CHECK_EQUALS(A.filename(), "")
    QUICK_CHECK_EQUALS(B.GetFilename(), "");

    A.assign("A/B.C");
    B.Assign("A/B.C");
    QUICK_CHECK_TRUE(A.has_filename());
    QUICK_CHECK_TRUE(B.IsFilenameValid());
    A.replace_filename("D");
    B.ReplaceFilename("D");
    QUICK_CHECK_EQUALS(A, "A/D")
    QUICK_CHECK_EQUALS(B, "A/D")

    A.assign("A/");
    B.Assign("A/");
    QUICK_CHECK_FALSE(A.has_filename());
    QUICK_CHECK_FALSE(B.IsFilenameValid());
    QUICK_CHECK_EQUALS(A.has_filename(), B.IsFilenameValid());
    A.replace_filename("B");
    B.ReplaceFilename("B");
    QUICK_CHECK_EQUALS(A, "A/B")
    QUICK_CHECK_EQUALS(B, "A/B")

    QUICK_CHECK_TRUE(A.has_parent_path())
    QUICK_CHECK_TRUE(B.IsParentValid())

    QUICK_CHECK_EQUALS(A.parent_path(), "A")
    QUICK_CHECK_EQUALS(B.GetParent(), "A");

    A.assign("A");
    B.Assign("A");
    QUICK_CHECK_FALSE(A.has_parent_path())
    QUICK_CHECK_FALSE(B.IsParentValid())

    QUICK_CHECK_EQUALS(A.parent_path(), "")
    QUICK_CHECK_EQUALS(B.GetParent(), "");

    A.assign("A/B");
    B.Assign("A/B");
    QUICK_CHECK_TRUE(A.has_stem())
    QUICK_CHECK_TRUE(B.IsStemValid())
    QUICK_CHECK_EQUALS(A.has_stem(), B.IsStemValid())
    QUICK_CHECK_EQUALS(A.stem(), "B")
    QUICK_CHECK_EQUALS(B.GetStem(), "B");

    A.assign("A/B.C.D");
    B.Assign("A/B.C.D");
    QUICK_CHECK_TRUE(A.has_stem())
    QUICK_CHECK_TRUE(B.IsStemValid())
    QUICK_CHECK_EQUALS(A.stem(), "B.C")
    QUICK_CHECK_EQUALS(B.GetStem(), "B.C");

    A.assign("A");
    B.Assign("A");
    QUICK_CHECK_TRUE(A.has_stem())
    QUICK_CHECK_TRUE(B.IsStemValid())
    QUICK_CHECK_EQUALS(A.has_stem(), B.IsStemValid())
    QUICK_CHECK_EQUALS(A.stem(), "A")
    QUICK_CHECK_EQUALS(B.GetStem(), "A");

    A.assign("");
    B.Assign("");
    QUICK_CHECK_FALSE(A.has_stem())
    QUICK_CHECK_FALSE(B.IsStemValid())
    QUICK_CHECK_EQUALS(A.has_stem(), B.IsStemValid())
    QUICK_CHECK_EQUALS(A.stem(), "")
    QUICK_CHECK_EQUALS(B.GetStem(), "")

    A.assign("A/");
    B.Assign("A/");
    QUICK_CHECK_FALSE(A.has_stem())
    QUICK_CHECK_FALSE(B.IsStemValid())
    QUICK_CHECK_EQUALS(A.has_stem(), B.IsStemValid())
    QUICK_CHECK_EQUALS(A.stem(), "")
    QUICK_CHECK_EQUALS(B.GetStem(), "")

    A.assign("A/.");
    B.Assign("A/.");
    QUICK_CHECK_TRUE(A.has_stem())
    QUICK_CHECK_TRUE(B.IsStemValid())
    QUICK_CHECK_EQUALS(A.has_stem(), B.IsStemValid())
    QUICK_CHECK_EQUALS(A.stem(), ".")
    QUICK_CHECK_EQUALS(B.GetStem(), ".")

    return;
}

TEST_CASE(Finder, "Lal.System")
{
    QUICK_CHECK_EQUALS(Finder::GetSavedDir(), "Saved")
    QUICK_CHECK_EQUALS(Finder::GetUserPreferencesFile(), "Saved/MyPreferences.cfg")
    QUICK_CHECK_EQUALS(Finder::GetSavesDir(), "Saved/Saves")
    QUICK_CHECK_EQUALS(Finder::GetDumpsDir(), "Saved/Dumps")
    QUICK_CHECK_EQUALS(Finder::GetMostRecentMemDumpFile(), "Saved/Dumps/proc.dmp")

    QUICK_CHECK_TRUE(Finder::DoesFileExist("jafg.jafgworkspace"))
    QUICK_CHECK_FALSE(Finder::DoesFileExist("jafg.jafgworkspace.unlikely"))

    return;
}
