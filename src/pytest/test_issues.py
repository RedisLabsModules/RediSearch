from common import getConnectionByEnv, waitForIndex, sortedResults, toSortedFlatList

def test_1282(env):
  env.expect('FT.CREATE idx ON HASH SCHEMA txt1 TEXT').equal('OK')
  env.expect('FT.ADD idx doc1 1.0 FIELDS txt1 foo').equal('OK')

  # optional search for new word would crash server
  env.expect('FT.SEARCH idx', '~foo').equal([1L, 'doc1', ['txt1', 'foo']])
  env.expect('FT.SEARCH idx', '~bar ~foo').equal([1L, 'doc1', ['txt1', 'foo']])

def test_1304(env):
  env.expect('FT.CREATE idx SCHEMA txt1 TEXT').equal('OK')
  env.expect('FT.EXPLAIN idx -20*').equal('PREFIX{-20*}\n')
  env.expect('FT.EXPLAIN idx -\\20*').equal('NOT{\n  PREFIX{20*}\n}\n')

def test_1414(env):
  env.skipOnCluster()
  env.expect('FT.CREATE idx SCHEMA txt1 TEXT').equal('OK')
  env.expect('ft.add idx doc 1 fields foo hello bar world').ok()
  env.expect('ft.search idx * limit 0 1234567').error().contains('LIMIT exceeds maximum of 1000000') 
  env.expect('FT.CONFIG set MAXSEARCHRESULTS -1').equal('OK')
  env.expect('ft.search idx * limit 0 1234567').equal([1L, 'doc', ['foo', 'hello', 'bar', 'world']]) 
