function values = test_vectors_get(name, type_name)
    %TEST_VECTORS_GET Read a vector from the test vector blob as a row vector
    %   type_name must match the type stored in the blob, for example 'double'.
    %   See test/data/TEST_VECTORS_FORMAT.md.

    % cSpell: ignore fileparts fullfile fopen fclose fread fseek ftell

    type_names = {'double', 'single', 'int8', 'uint8', 'int16', 'uint16', 'int32', 'uint32', 'int64', 'uint64'};
    element_sizes = [8, 4, 1, 1, 2, 2, 4, 4, 8, 8];
    type_code = find(strcmp(type_names, type_name));
    assert(~isempty(type_code), 'Unknown test vector type: %s', type_name);

    blob_path = fullfile(fileparts(mfilename('fullpath')), '..', 'data', 'test_vectors.bin');
    [fid, message] = fopen(blob_path, 'rb', 'ieee-le');
    assert(fid ~= -1, 'Could not open %s: %s', blob_path, message);
    closer = onCleanup(@() fclose(fid));

    fseek(fid, 0, 'eof');
    actual_size = ftell(fid);
    fseek(fid, 0, 'bof');

    magic = fread(fid, 8, 'uint8=>char')';
    assert(strcmp(magic, 'RRTVECTS'), 'Invalid test vector blob');
    header = fread(fid, 4, 'uint32=>double');
    assert(numel(header) == 4 && header(1) == 1 && header(2) == 64 && header(4) == actual_size, ...
        'Invalid or unsupported test vector blob');
    entry_count = header(3);
    assert(24 + 64 * entry_count <= actual_size, 'Truncated test vector index');

    for i = 1:entry_count
        fseek(fid, 24 + 64 * (i - 1), 'bof');
        raw_name = fread(fid, 48, 'uint8=>char')';
        entry = fread(fid, 3, 'uint32=>double');
        terminator = find(raw_name == char(0), 1);
        if ~isempty(terminator)
            raw_name = raw_name(1:terminator - 1);
        end
        if ~strcmp(raw_name, name)
            continue;
        end

        assert(entry(1) == type_code, 'Test vector type mismatch: %s', name);
        assert(entry(3) + entry(2) * element_sizes(type_code) <= actual_size, ...
            'Test vector out of bounds: %s', name);
        fseek(fid, entry(3), 'bof');
        values = fread(fid, entry(2), ['*' type_name])';
        return;
    end

    error('Test vector not found: %s', name);
end
