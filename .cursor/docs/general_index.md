# General Index

## bin/

- `blessed-modifier-order.sh` - CLI tool to canonicalize Java modifier ordering in source files [CLI]
- `idea.sh` - Generates an IntelliJ IDEA project from JDK modules [CLI]
- `jib.sh` - Bootstrapper that installs and runs the JIB configuration tool [CLI]

## make/jdk/src/classes/build/tools/pandocfilter/

- `PandocFilter.java` - Utility to traverse and transform Pandoc JSON AST nodes. Key: `PandocFilter`, `traverse(JSONValue, Callback, boolean)`, `createPandocNode(String, JSONValue)`, `loadJson(String[])`, `Callback` [SOURCE_CODE]

## src/java.base/aix/classes/sun/nio/fs/

- `AixFileSystemProvider.java` - AIX-specific FileSystemProvider implementing file store and user-defined attribute view support. Key: `AixFileSystemProvider`, `newFileSystem`, `getFileStore`, `supportsUserDefinedFileAttributeView`, `getFileAttributeView` [SOURCE_CODE]

## src/java.base/share/classes/

- `module-info.java` - Module declaration for the java.base module. Key: `module java.base`, `exports`, `uses / provides` [SOURCE_CODE]

## src/java.base/share/classes/java/io/

- `Console.java` - API for interacting with the character-based console device. Key: `Console`, `writer`, `reader`, `readLine / readPassword`, `format / printf` [SOURCE_CODE]
- `File.java` - Abstract representation of file and directory pathnames. Key: `File`, `FS`, `separatorChar / separator / pathSeparatorChar / pathSeparator`, `isInvalid()`, `toPath()` [SOURCE_CODE]
- `FileInputStream.java` - File-based InputStream implementation with JFR hooks. Key: `FileInputStream`, `read / readBytes / read0`, `readAllBytes / readNBytes`, `transferTo`, `open0 / length0 / position0 / skip0` [SOURCE_CODE]
- `FileOutputStream.java` - File-based OutputStream implementation with JFR tracing. Key: `FileOutputStream`, `write / writeBytes / writeBytes(native)`, `getChannel`, `open0`, `close` [SOURCE_CODE]
- `RandomAccessFile.java` - Public java.io.RandomAccessFile implementation providing random-access I/O and a FileChannel.. Key: `RandomAccessFile`, `open0`, `open`, `getChannel`, `read` [SOURCE_CODE]
- `Reader.java` - Abstract base class for character stream readers. Key: `Reader`, `nullReader`, `of`, `read(char[] cbuf, int off, int len)`, `readAllLines / readAllAsString` [SOURCE_CODE]

## src/java.base/share/classes/java/lang/

- `CharacterData.java` - Abstract character data dispatch and API for Unicode properties. Key: `CharacterData`, `of`, `getType / isUpperCase / toLowerCase / digit` [SOURCE_CODE]
- `CharacterDataPrivateUse.java` - CharacterData implementation for private-use code points. Key: `CharacterDataPrivateUse`, `instance`, `getType / toLowerCase / isEmoji` [SOURCE_CODE]
- `CharacterDataUndefined.java` - CharacterData for undefined Unicode code points. Key: `CharacterDataUndefined`, `instance` [SOURCE_CODE]
- `IO.java` - Convenience line-oriented I/O helpers for System.in/out. Key: `IO`, `readln`, `print / println`, `reader` [SOURCE_CODE]
- `InheritableThreadLocal.java` - ThreadLocal variant that inherits values to child threads. Key: `InheritableThreadLocal`, `childValue(T)`, `getMap(Thread)`, `createMap(Thread, T)` [SOURCE_CODE]
- `Integer.java` - Wrapper and utilities for primitive int values. Key: `Integer`, `toString(int)`, `toString(int, int)`, `toUnsignedString(int, int)`, `digits` [SOURCE_CODE]
- `Long.java` - Wrapper and utilities for primitive long values. Key: `Long`, `toString(long)`, `toUnsignedString(long, int)`, `toHexString(long)` [SOURCE_CODE]
- `Process.java` - Abstract API for controlling native processes. Key: `Process`, `getOutputStream/getInputStream/getErrorStream`, `inputReader/errorReader/outputWriter`, `onExit`, `pid/info/children/descendants` [SOURCE_CODE]
- `ScopedValue.java` - API for binding values to dynamic scoped execution contexts. Key: `ScopedValue`, `ScopedValue.Carrier`, `ScopedValue.Snapshot`, `Carrier.run / Carrier.call`, `CallableOp` [SOURCE_CODE]
- `StringConcatHelper.java` - Low-level helpers for optimized string concatenation. Key: `StringConcatHelper`, `StringConcatHelper.Concat1`, `mix / prepend / newString / doConcat / simpleConcat`, `stringOf` [SOURCE_CODE]
- `ThreadLocal.java` - Per-thread variable storage with map implementation. Key: `ThreadLocal`, `ThreadLocal.withInitial`, `ThreadLocal.ThreadLocalMap`, `ThreadLocalMap.Entry`, `createInheritedMap / childValue` [SOURCE_CODE]
- `Throwable.java` - Root class for exceptions and errors with stack/cause handling. Key: `Throwable`, `initCause/getCause`, `fillInStackTrace/getStackTrace/setStackTrace`, `addSuppressed/getSuppressed`, `SentinelHolder / UNASSIGNED_STACK` [SOURCE_CODE]

## src/java.base/share/classes/java/lang/classfile/

- `Signature.java` - Model for Java generic type signatures (JVMS). Key: `Signature`, `Signature.parseFrom(String)`, `ClassTypeSig`, `TypeArg`, `ArrayTypeSig` [SOURCE_CODE]

## src/java.base/share/classes/java/lang/runtime/

- `SwitchBootstraps.java` - Bootstrap implementations for invokedynamic-based switch forms. Key: `typeSwitch`, `enumSwitch`, `generateTypeSwitch`, `MappedEnumCache`, `ResolvedEnumLabels` [SOURCE_CODE]

## src/java.base/share/classes/java/net/

- `Authenticator.java` - Hook for obtaining network authentication credentials. Key: `Authenticator`, `Authenticator.setDefault / getDefault`, `requestPasswordAuthentication(...)`, `getPasswordAuthentication`, `RequestorType` [SOURCE_CODE]
- `HostPortrange.java` - Parses host (including IPv6) and port or port-range strings. Key: `HostPortrange`, `toLowerCase`, `parsePort`, `defaultPort / HTTP_PORT / HTTPS_PORT / NO_PORT` [SOURCE_CODE]
- `Inet4AddressImpl.java` - IPv4-specific native host/address lookup implementation. Key: `Inet4AddressImpl`, `getLocalHostName / lookupAllHostAddr / getHostByAddr`, `anyLocalAddress / loopbackAddress`, `isReachable` [SOURCE_CODE]
- `Inet6Address.java` - Representation and utilities for IPv6 addresses. Key: `Inet6Address`, `Inet6Address.Inet6AddressHolder`, `getByAddress(host, addr, nif) / getByAddress(host, addr, scope_id)`, `numericToTextFormat / textToNumericFormat utilities` [SOURCE_CODE]
- `NetworkInterface.java` - API to enumerate and query system network interfaces. Key: `NetworkInterface`, `getNetworkInterfaces / networkInterfaces`, `getByName / getByIndex / getByInetAddress`, `isUp / isLoopback / isVirtual / supportsMulticast` [SOURCE_CODE]
- `Proxy.java` - Immutable representation of a network proxy configuration. Key: `Proxy`, `Proxy.Type`, `NO_PROXY`, `equals / hashCode / toString` [SOURCE_CODE]
- `Socket.java` - Client socket implementation with stream support. Key: `Socket`, `STATE / state flags`, `createImpl()`, `getImpl()`, `connect / bind / close` [SOURCE_CODE]
- `SocksSocketImpl.java` - SOCKS (v4 & v5) proxy-enabled socket implementation. Key: `SocksSocketImpl`, `connect(SocketAddress,int)`, `authenticate(byte, InputStream, BufferedOutputStream, long)`, `connectV4 / readSocksReply / doConnect` [SOURCE_CODE]
- `URLClassLoader.java` - ClassLoader that loads classes/resources from URLs and JARs. Key: `URLClassLoader`, `getResourceAsStream / getResource`, `findClass / defineClass(Resource)`, `close` [SOURCE_CODE]
- `URLStreamHandler.java` - Abstract protocol handler for URL connections and parsing. Key: `URLStreamHandler`, `openConnection(URL) / openConnection(URL, Proxy)`, `parseURL`, `sameFile / equals / hashCode / hostsEqual` [SOURCE_CODE]

## src/java.base/share/classes/java/nio/file/

- `FileSystems.java` - Factory and accessors for default and non-default FileSystem instances using FileSystemProviders. Key: `FileSystems`, `DefaultFileSystemHolder`, `DefaultFileSystemHolder.defaultFileSystem`, `DefaultFileSystemHolder.getDefaultProvider`, `getDefault` [SOURCE_CODE]

## src/java.base/share/classes/java/security/

- `AsymmetricKey.java` - Interface representing an asymmetric (public/private) key. Key: `AsymmetricKey`, `getParams` [SOURCE_CODE]
- `DEREncodable.java` - Sealed marker interface for DER-encodable cryptographic objects. Key: `DEREncodable` [SOURCE_CODE]
- `KeyPair.java` - Simple holder for a public/private key pair. Key: `KeyPair`, `getPublic`, `getPrivate` [SOURCE_CODE]
- `PEMDecoder.java` - Immutable, thread-safe PEM data decoder that maps PEM types to Java crypto objects.. Key: `PEMDecoder`, `of`, `decode`, `decode`, `getKeyFactory` [SOURCE_CODE]
- `PEMEncoder.java` - PEM encoder for various DER-encoded security objects, with optional PBE encryption for private keys.. Key: `PEMEncoder`, `PEMEncoder`, `of`, `encodeToString`, `encode` [SOURCE_CODE]
- `PEMRecord.java` - A Java record representing PEM (RFC 7468) data with type, Base64 content, and optional leading non‑PEM bytes.. Key: `PEMRecord`, `PEMRecord(String,String)`, `toString` [SOURCE_CODE]
- `SecureClassLoader.java` - ClassLoader extension that associates classes with ProtectionDomains. Key: `SecureClassLoader`, `defineClass(String, byte[], int, int, CodeSource)`, `getPermissions`, `CodeSourceKey`, `resetArchivedStates` [SOURCE_CODE]

## src/java.base/share/classes/java/security/cert/

- `X509CRL.java` - Abstract base for X.509 Certificate Revocation Lists (CRLs). Key: `X509CRL`, `getEncoded()`, `verify(PublicKey)`, `getRevokedCertificate(BigInteger)`, `getIssuerX500Principal` [SOURCE_CODE]
- `X509Certificate.java` - Abstract base for X.509 public key certificates. Key: `X509Certificate`, `checkValidity() / checkValidity(Date)`, `getVersion()`, `getSerialNumber()`, `getIssuerX500Principal() / getSubjectX500Principal()` [SOURCE_CODE]

## src/java.base/share/classes/java/text/

- `DigitList.java` - Internal helper for decimal digit representation and rounding. Key: `DigitList`, `getDouble`, `getLong`, `getBigDecimal`, `set(boolean,double,int,boolean)` [SOURCE_CODE]

## src/java.base/share/classes/java/time/

- `MonthDay.java` - Immutable value type representing a month-day (no year). Key: `MonthDay`, `of(Month,int)`, `parse`, `from` [SOURCE_CODE]

## src/java.base/share/classes/java/util/

- `AbstractMap.java` - Skeletal AbstractMap implementation providing default Map behaviors. Key: `AbstractMap`, `entrySet`, `keySet`, `values`, `equals / hashCode` [SOURCE_CODE]
- `Locale.java` - Core Locale implementation representing language, script, region, variant, and extensions (BCP 47/LDML aware).. Key: `Locale`, `Builder`, `LanguageRange`, `Category`, `UNICODE_LOCALE_EXTENSION` [SOURCE_CODE]
- `LocaleISOData.java` - Static tables of ISO language and country codes and helpers. Key: `isoLanguageTable`, `isoCountryTable`, `computeISO3166_1Alpha3Countries` [SOURCE_CODE]
- `PropertyResourceBundle.java` - ResourceBundle implementation backed by .properties files. Key: `PropertyResourceBundle`, `PropertyResourceBundle(InputStream)`, `PropertyResourceBundle(Reader)`, `handleGetObject`, `getKeys` [SOURCE_CODE]
- `ReverseOrderListView.java` - List view that presents a list in reverse encounter order. Key: `ReverseOrderListView`, `of`, `DescendingIterator`, `DescendingListIterator`, `checkModifiable` [SOURCE_CODE]
- `SequencedMap.java` - Map interface with well-defined encounter order and deque-like ops. Key: `SequencedMap`, `firstEntry / lastEntry / pollFirstEntry / pollLastEntry`, `sequencedKeySet / sequencedValues / sequencedEntrySet`, `reversed` [SOURCE_CODE]
- `UUID.java` - Immutable representation and utilities for UUIDs. Key: `UUID`, `randomUUID`, `nameUUIDFromBytes`, `fromString / toString`, `NIBBLES` [SOURCE_CODE]

## src/java.base/share/classes/java/util/concurrent/

- `AbstractExecutorService.java` - Base ExecutorService implementations for submit/invoke operations. Key: `AbstractExecutorService`, `newTaskFor(Runnable, T)`, `newTaskFor(Callable<T>)`, `doInvokeAny(Collection<? extends Callable<T>>, boolean, long)`, `invokeAll(Collection<? extends Callable<T>>...)` [SOURCE_CODE]
- `ExecutorCompletionService.java` - CompletionService that enqueues completed task Futures. Key: `ExecutorCompletionService`, `QueueingFuture`, `submit(Callable<V>)`, `take/poll/poll(long, TimeUnit)` [SOURCE_CODE]
- `StructuredTaskScopeImpl.java` - Implementation of StructuredTaskScope for structured concurrency. Key: `StructuredTaskScopeImpl`, `SubtaskImpl`, `ConfigImpl`, `fork(Callable<? extends U>)`, `join()` [SOURCE_CODE]

## src/java.base/share/classes/java/util/stream/

- `GathererOp.java` - Stream pipeline operation implementing Gatherer-based gathering. Key: `GathererOp`, `NodeBuilder`, `GatherSink`, `evaluate(...)`, `Hybrid` [SOURCE_CODE]

## src/java.base/share/classes/java/util/zip/

- `Adler32.java` - Adler-32 checksum implementation. Key: `Adler32`, `update(int)`, `update(byte[], int, int)`, `update(ByteBuffer)` [SOURCE_CODE]
- `CRC32.java` - CRC-32 checksum implementation. Key: `CRC32`, `update(int)`, `update(byte[], int, int)`, `update(ByteBuffer)` [SOURCE_CODE]
- `CRC32C.java` - Optimized CRC-32C (Castagnoli) implementation. Key: `CRC32C`, `byteTables / byteTable0..7`, `update(int)`, `updateBytes(int, byte[], int, int)`, `updateDirectByteBuffer(int, long, int, int)` [SOURCE_CODE]
- `Deflater.java` - ZLIB deflater wrapper for compressing data. Key: `Deflater`, `setInput(byte[], int, int) / setInput(ByteBuffer)`, `deflate(...)`, `setDictionary(...)`, `finish()/finished()/needsInput()` [SOURCE_CODE]
- `Inflater.java` - ZLIB inflater wrapper for decompressing data. Key: `Inflater`, `setInput(byte[], int, int) / setInput(ByteBuffer)`, `inflate(byte[], int, int) / inflate(ByteBuffer)`, `setDictionary(...)`, `needsInput()/needsDictionary()/finished()` [SOURCE_CODE]

## src/java.base/share/classes/javax/crypto/

- `EncryptedPrivateKeyInfo.java` - Represents and manipulates PKCS#8 EncryptedPrivateKeyInfo structures. Key: `EncryptedPrivateKeyInfo`, `EncryptedPrivateKeyInfo(byte[])`, `getKeySpec(Cipher)`, `encryptKey / encryptKeyImpl` [SOURCE_CODE]
- `KEM.java` - API wrapper for Key Encapsulation Mechanism (KEM) operations. Key: `KEM`, `KEM.Encapsulated`, `KEM.Encapsulator`, `KEM.Decapsulator`, `DelayedKEM` [SOURCE_CODE]

## src/java.base/share/classes/javax/net/ssl/

- `ExtendedSSLSession.java` - Extended SSLSession interface with extra session attributes. Key: `ExtendedSSLSession`, `getLocalSupportedSignatureAlgorithms`, `getPeerSupportedSignatureAlgorithms`, `getRequestedServerNames`, `exportKeyingMaterialKey / exportKeyingMaterialData` [SOURCE_CODE]

## src/java.base/share/classes/javax/security/auth/

- `Subject.java` - Represents an authenticated subject with principals and credentials. Key: `Subject`, `current()`, `callAs(Subject, Callable<T>)`, `doAs / doAsPrivileged`, `SecureSet` [SOURCE_CODE]

## src/java.base/share/classes/jdk/internal/access/

- `JavaLangAccess.java` - Internal low-level accessors for core java.lang operations. Key: `JavaLangAccess`, `defineClass`, `getConstantPool`, `addExports/addOpens/addReads`, `uncheckedNewStringNoRepl / uncheckedGetBytesNoRepl` [SOURCE_CODE]

## src/java.base/share/classes/jdk/internal/event/

- `ExceptionThrownEvent.java` - JFR event type for thrown exceptions [GENERATED]
- `FileReadEvent.java` - JFR event for file read operations with offer helper [GENERATED]
- `FileWriteEvent.java` - JFR event for file write operations with offer helper [GENERATED]
- `SocketReadEvent.java` - JFR event helper for socket read operations. Key: `SocketReadEvent`, `commit`, `offer`, `emit`, `shouldThrottleCommit` [SOURCE_CODE]
- `SocketWriteEvent.java` - JFR event helper for socket write operations. Key: `SocketWriteEvent`, `commit`, `offer`, `emit`, `shouldThrottleCommit` [SOURCE_CODE]
- `ThrowableTracer.java` - Helper for tracing thrown errors/exceptions to JFR. Key: `ThrowableTracer`, `traceError`, `traceThrowable`, `emitStatistics`, `numThrowables` [SOURCE_CODE]

## src/java.base/share/classes/jdk/internal/foreign/

- `SegmentFactories.java` - Factories for creating MemorySegment implementations. Key: `makeNativeSegmentUnchecked`, `allocateNativeSegment`, `allocateNativeInternal`, `ensureInitialized`, `fromArray` [SOURCE_CODE]

## src/java.base/share/classes/jdk/internal/io/

- `JdkConsoleImpl.java` - Platform TTY-based implementation of JdkConsole. Key: `JdkConsoleImpl`, `readPassword`, `readLine`, `LineReader`, `echo` [SOURCE_CODE]
- `JdkConsoleProvider.java` - Service provider interface for JdkConsole implementations. Key: `DEFAULT_PROVIDER_MODULE_NAME`, `console`, `JdkConsoleProvider` [SOURCE_CODE]

## src/java.base/share/classes/jdk/internal/javac/

- `PreviewFeature.java` - Annotation used by javac to mark API declarations as associated with a preview feature and carry JEP metadata.. Key: `PreviewFeature`, `feature`, `reflective`, `Feature`, `JEP` [SOURCE_CODE]

## src/java.base/share/classes/jdk/internal/misc/

- `CDS.java` - Utilities and helpers for Class Data Sharing (CDS). Key: `IS_DUMPING_ARCHIVE`, `configStatus`, `isDumpingArchive`, `dumpSharedArchive`, `generateLambdaFormHolderClasses` [SOURCE_CODE]
- `CarrierThreadLocal.java` - ThreadLocal variant bound to carrier threads. Key: `CarrierThreadLocal`, `get`, `set`, `remove`, `JLA` [SOURCE_CODE]
- `TerminatingThreadLocal.java` - ThreadLocal notified on thread termination. Key: `TerminatingThreadLocal`, `threadTerminated`, `REGISTRY`, `register`, `threadTerminated` [SOURCE_CODE]

## src/java.base/share/classes/jdk/internal/util/

- `DecimalDigits.java` - Efficient decimal digit packing and conversion utilities. Key: `DIGITS`, `stringSize`, `uncheckedGetCharsLatin1`, `uncheckedGetCharsUTF16`, `getChars` [SOURCE_CODE]
- `Exceptions.java` - Sensitive-info-aware exception message helpers. Key: `Exceptions`, `SensitiveInfo`, `filterSocketInfo`, `formatMsg`, `ioException` [SOURCE_CODE]
- `HexDigits.java` - Precomputes and provides fast access to ASCII hex digit pairs for bytes (0–255).. Key: `HexDigits`, `DIGITS`, `digitPair` [SOURCE_CODE]

## src/java.base/share/classes/jdk/internal/vm/

- `ThreadDumper.java` - Produces plain-text or JSON thread dumps to files or byte arrays for VM diagnostics. Key: `ThreadDumper`, `MAX_BYTE_ARRAY_SIZE`, `dumpThreads`, `dumpThreadsToJson`, `dumpThreadsToByteArray` [SOURCE_CODE]
- `ThreadSnapshot.java` - Java-side representation of a thread snapshot populated by the VM for inspection and tooling.. Key: `ThreadSnapshot`, `of`, `create`, `ThreadLock`, `ThreadBlocker` [SOURCE_CODE]

## src/java.base/share/classes/sun/net/util/

- `IPAddressUtil.java` - Utilities for parsing and handling IP address literals. Key: `textToNumericFormatV4`, `textToNumericFormatV6`, `validateNumericFormatV4`, `toScopedAddress`, `match/scan` [SOURCE_CODE]
- `ProxyUtil.java` - Utility for copying Proxy objects safely. Key: `copyProxy`, `ProxyUtil` [SOURCE_CODE]

## src/java.base/share/classes/sun/net/www/

- `ParseUtil.java` - URL/URI encoding, decoding and quoting utilities. Key: `encodePath`, `decode`, `fileToEncodedURL`, `toURI`, `quote` [SOURCE_CODE]

## src/java.base/share/classes/sun/nio/ch/

- `DatagramSocketAdaptor.java` - Multicast DatagramSocket wrapper around a DatagramChannel. Key: `DatagramSocketAdaptor`, `send`, `receive`, `joinGroup`, `leaveGroup` [SOURCE_CODE]
- `IOUtil.java` - File-descriptor based native I/O helpers for ByteBuffer operations. Key: `IOUtil`, `write(FileDescriptor, ByteBuffer, long, ...)`, `read(FileDescriptor, ByteBuffer, long, ...)`, `IOV_MAX`, `WRITEV_MAX` [SOURCE_CODE]
- `IOVecWrapper.java` - Wrapper managing native iovec arrays for scatter/gather I/O. Key: `IOVecWrapper`, `get(int)`, `putBase(int,long) / putLen(int,long)`, `addressSize` [SOURCE_CODE]
- `Net.java` - Network utilities, socket creation and socket option handling. Key: `Net`, `setSocketOption(FileDescriptor, ProtocolFamily, SocketOption, Object)`, `getSocketOption(FileDescriptor, ProtocolFamily, SocketOption)`, `socket / serverSocket`, `translateException / translateToSocketException` [SOURCE_CODE]
- `NioSocketImpl.java` - NIO-based SocketImpl providing socket I/O, timeouts and virtual-thread aware non-blocking behavior. Key: `NioSocketImpl`, `MAX_BUFFER_SIZE`, `create`, `beginRead`, `endRead` [SOURCE_CODE]
- `SocketAdaptor.java` - Adapter exposing a SocketChannel as a java.net.Socket. Key: `SocketAdaptor`, `create(SocketChannelImpl)`, `getInputStream / getOutputStream`, `setTcpNoDelay / setSoLinger / setSoTimeout` [SOURCE_CODE]

## src/java.base/share/classes/sun/security/ec/

- `ECKeyFactory.java` - Provider KeyFactorySpi implementation for EC keys (translate/generate/validate EC keys).. Key: `ECKeyFactory`, `toECKey`, `checkKey`, `engineTranslateKey`, `engineGeneratePublic` [SOURCE_CODE]
- `ECPrivateKeyImpl.java` - Implementation of EC private keys (PKCS#8) and public key derivation. Key: `ECPrivateKeyImpl`, `parseKeyBits()`, `getS() / getArrayS()`, `calculatePublicKey()` [SOURCE_CODE]
- `XDHKeyFactory.java` - KeyFactorySpi implementation for XDH (X25519/X448) keys; converts/generates Key and KeySpec forms.. Key: `XDHKeyFactory`, `lockedParams`, `engineTranslateKey`, `engineGeneratePublic`, `engineGeneratePrivate` [SOURCE_CODE]
- `XDHPrivateKeyImpl.java` - XDH (X25519/X448) PKCS#8 private key implementation. Key: `XDHPrivateKeyImpl`, `getK()`, `calculatePublicKey()`, `checkLength(XECParameters)` [SOURCE_CODE]

## src/java.base/share/classes/sun/security/ec/ed/

- `EdDSAKeyFactory.java` - KeyFactorySpi for EdDSA (Ed25519/Ed448) key conversion and generation. Key: `EdDSAKeyFactory`, `engineTranslateKey`, `engineGeneratePublic / engineGeneratePrivate`, `Ed25519 / Ed448` [SOURCE_CODE]

## src/java.base/share/classes/sun/security/pkcs/

- `NamedPKCS8Key.java` - PKCS#8 private key for algorithms specialized by a named parameter set. Key: `NamedPKCS8Key`, `getRawBytes()`, `getParams()`, `destroy() / isDestroyed()` [SOURCE_CODE]
- `PKCS8Key.java` - Representation and parsing/encoding of PKCS#8 private keys (v1 and v2) with serialization support. Key: `PKCS8Key`, `decode`, `parseKey`, `generateEncoding`, `getEncodedInternal` [SOURCE_CODE]

## src/java.base/share/classes/sun/security/provider/

- `DSAPrivateKey.java` - DSA PKCS#8 private key implementation. Key: `DSAPrivateKey`, `getParams()`, `getX()` [SOURCE_CODE]
- `KeyProtector.java` - Password-based proprietary key protector used by legacy keystores. Key: `KeyProtector`, `protect(Key)`, `recover(EncryptedPrivateKeyInfo)`, `SALT_LEN / DIGEST_ALG / DIGEST_LEN` [SOURCE_CODE]
- `X509Factory.java` - CertificateFactorySpi implementation for parsing, caching, and interning X.509 certs and CRLs. Key: `X509Factory`, `BEGIN_CERT`, `END_CERT`, `ENC_MAX_LENGTH`, `certCache` [SOURCE_CODE]

## src/java.base/share/classes/sun/security/rsa/

- `RSAKeyFactory.java` - KeyFactorySpi implementation for RSA/RSASSA-PSS: translate, validate and generate RSA keys and KeySpecs.. Key: `RSAKeyFactory`, `getInstance`, `checkKeyAlgo`, `toRSAKey`, `checkRSAProviderKeyLengths` [SOURCE_CODE]
- `RSAPrivateCrtKeyImpl.java` - RSA CRT private key implementation and PKCS#1/PKCS#8 parsing. Key: `RSAPrivateCrtKeyImpl`, `newKey(KeyType,String,byte[])`, `parsePKCS1(byte[])`, `getModulus / getPrivateExponent / getPrimeP / getPrimeQ` [SOURCE_CODE]
- `RSAPrivateKeyImpl.java` - Non-CRT RSA private key implementation (modulus & exponent). Key: `RSAPrivateKeyImpl`, `getModulus / getPrivateExponent` [SOURCE_CODE]
- `RSAPublicKeyImpl.java` - RSA public key implementation with X.509/PKCS#1 parsing and validation. Key: `RSAPublicKeyImpl`, `newKey(KeyType,String,byte[])`, `parsePKCS1(byte[])`, `checkExponentRange(BigInteger,BigInteger)` [SOURCE_CODE]

## src/java.base/share/classes/sun/security/ssl/

- `CertificateVerify.java` - TLS CertificateVerify handshake message production and consumption. Key: `CertificateVerify`, `S30CertificateVerifyMessage / T10CertificateVerifyMessage / T12CertificateVerifyMessage / T13CertificateVerifyMessage`, `getSignature(String, Key)`, `S30CertificateVerifyProducer / Consumer (and equivalents for other versions)` [SOURCE_CODE]
- `Finished.java` - Implements creation and verification of the TLS/SSL Finished handshake message across protocol versions.. Key: `Finished`, `FinishedMessage`, `VerifyDataGenerator`, `VerifyDataScheme`, `S30VerifyDataGenerator` [SOURCE_CODE]
- `PreSharedKeyExtension.java` - TLS pre_shared_key extension handling for PSK resumption and binders. Key: `PreSharedKeyExtension`, `CHPreSharedKeySpec / SHPreSharedKeySpec`, `PskIdentity`, `CHPreSharedKeyConsumer / CHPreSharedKeyProducer` [SOURCE_CODE]
- `SessionTicketExtension.java` - TLS session ticket extension and stateless ticket encryption/decryption. Key: `SessionTicketExtension`, `SessionTicketSpec`, `StatelessKey`, `KeyState.getCurrentKey / getKey` [SOURCE_CODE]

## src/java.base/share/classes/sun/security/util/

- `Debug.java` - Security debugging helper that formats and emits debug output. Key: `class Debug`, `getInstance`, `println`, `toHexString / toString(byte[])` [SOURCE_CODE]
- `DerValue.java` - Represents and parses individual DER/ASN.1 encoded values. Key: `class DerValue`, `DerValue(byte[] buf, int offset, int len, boolean allowBER, boolean allowMore)`, `encode`, `tag constants (e.g., tag_Integer, tag_Sequence)` [SOURCE_CODE]
- `DomainName.java` - Determines public suffixes and registered domains from domain names. Key: `class DomainName`, `class Rules`, `class RuleSet`, `registeredDomain` [SOURCE_CODE]
- `KeyUtil.java` - Key utilities for size, validation and algorithm extraction. Key: `class KeyUtil`, `getKeySize(Key)`, `validate(Key) / validate(KeySpec)`, `checkTlsPreMasterSecretKey` [SOURCE_CODE]
- `KnownOIDs.java` - Registry mapping well-known algorithm and attribute names to OID strings. Key: `enum KnownOIDs`, `findMatch`, `value` [SOURCE_CODE]
- `Pem.java` - Utility for parsing and producing PEM-format blocks (RFC 1421/7468) used by JDK security code. Key: `Pem`, `DEFAULT_ALGO`, `PBE_PATTERN`, `STRIP_WHITESPACE_PATTERN`, `PBES2OID` [SOURCE_CODE]

## src/java.base/share/classes/sun/security/x509/

- `X509Key.java` - Represents and parses X.509 SubjectPublicKeyInfo keys. Key: `class X509Key`, `parse(DerValue)`, `buildX509Key`, `encode / decode` [SOURCE_CODE]

## src/java.base/share/classes/sun/util/locale/

- `InternalLocaleBuilder.java` - Builder for internal locale components and BCP47 extensions. Key: `class InternalLocaleBuilder`, `setLanguage / setScript / setRegion / setVariant`, `setUnicodeLocaleKeyword / addUnicodeLocaleAttribute`, `setLanguageTag / setLocale` [SOURCE_CODE]
- `LanguageTag.java` - Parser and canonicalizer for BCP47 language tags. Key: `record LanguageTag`, `parse(String, ParsePosition, boolean)`, `caseFoldTag`, `LEGACY map` [SOURCE_CODE]

## src/java.base/unix/classes/sun/nio/ch/

- `UnixAsynchronousSocketChannelImpl.java` - Unix AIO socket channel implementation using Port/epoll polling. Key: `class UnixAsynchronousSocketChannelImpl`, `implConnect / implRead / implWrite`, `onEvent` [SOURCE_CODE]

## src/java.base/unix/classes/sun/nio/fs/

- `UnixNativeDispatcher.java` - Native wrapper for Unix system and libc calls used by NIO FS. Key: `class UnixNativeDispatcher`, `copyToNativeBuffer`, `open / stat / lstat / readlink / symlink / mkdir / unlink` [SOURCE_CODE]
- `UnixSecureDirectoryStream.java` - Unix implementation of SecureDirectoryStream using directory fds. Key: `UnixSecureDirectoryStream`, `newDirectoryStream`, `newByteChannel`, `BasicFileAttributeViewImpl / PosixFileAttributeViewImpl` [SOURCE_CODE]
- `UnixUserDefinedFileAttributeView.java` - Unix implementation of UserDefinedFileAttributeView using xattrs. Key: `class UnixUserDefinedFileAttributeView`, `list / size / read / write / delete`, `copyExtendedAttributes` [SOURCE_CODE]
- `UnixUserPrincipals.java` - Lookup and representation of Unix user/group principals. Key: `class User`, `fromUid / fromGid`, `lookupUser / lookupGroup` [SOURCE_CODE]

## src/java.base/windows/classes/java/io/

- `WinNTFileSystem.java` - Windows NT/2000 filesystem path normalization and resolution. Key: `WinNTFileSystem`, `LONG_PATH_PREFIX`, `normalize(String)`, `resolve(String,String)`, `isInvalid(File)` [SOURCE_CODE]

## src/java.base/windows/classes/sun/nio/ch/

- `WindowsAsynchronousSocketChannelImpl.java` - Windows AIO socket channel using IOCP and overlapped I/O. Key: `class WindowsAsynchronousSocketChannelImpl`, `ConnectTask / ReadTask / WriteTask`, `getByOverlapped` [SOURCE_CODE]

## src/java.base/windows/classes/sun/nio/fs/

- `WindowsSecurityDescriptor.java` - Constructs and decodes Windows security descriptors and ACLs. Key: `class WindowsSecurityDescriptor`, `create(List<AclEntry>)`, `getAcl` [SOURCE_CODE]
- `WindowsUserPrincipals.java` - Lookup and representation of Windows user/group principals. Key: `class User`, `fromSid`, `lookup` [SOURCE_CODE]

## src/java.compiler/share/classes/javax/annotation/processing/

- `FilerException.java` - IOException signaling Filer contract violations. Key: `FilerException`, `serialVersionUID`, `FilerException(String)` [SOURCE_CODE]

## src/java.compiler/share/classes/javax/lang/model/

- `UnknownEntityException.java` - Runtime exception for unknown language entities. Key: `UnknownEntityException`, `serialVersionUID`, `UnknownEntityException(String)` [SOURCE_CODE]

## src/java.desktop/macosx/classes/sun/font/

- `CCharToGlyphMapper.java` - macOS CoreText-backed char->glyph mapper with multi-layer cache. Key: `CCharToGlyphMapper`, `countGlyphs`, `nativeCharsToGlyphs`, `Cache`, `SparseBitShiftingTwoLayerArray` [SOURCE_CODE]

## src/java.desktop/macosx/classes/sun/lwawt/macosx/

- `CAccessibility.java` - macOS accessibility bridge for Swing components. Key: `CAccessibility`, `getAccessibility(String[])`, `focusChanged`, `invokeAndWait / invokeLater`, `getAccessibleName/getAccessibleRole/getAccessibleDescription` [SOURCE_CODE]
- `CRobot.java` - macOS RobotPeer implementing synthetic input and screen capture. Key: `CRobot`, `mouseMove / mousePress / mouseRelease`, `keyPress / keyRelease`, `initRobot / mouseEvent / keyEvent / nativeGetScreenPixels`, `getRGBPixels` [SOURCE_CODE]

## src/java.desktop/share/classes/

- `module-info.java` - Module descriptor for java.desktop (AWT/Swing) APIs. Key: `module java.desktop`, `requires transitive java.datatransfer, java.xml`, `exports java.awt, javax.swing, ...` [SOURCE_CODE]

## src/java.desktop/share/classes/java/awt/

- `BorderLayout.java` - AWT BorderLayout manager arranging five regions of a container. Key: `BorderLayout`, `NORTH,SOUTH,EAST,WEST,CENTER`, `addLayoutComponent(java.awt.Component, Object)`, `layoutContainer` [SOURCE_CODE]
- `FlowLayout.java` - AWT FlowLayout manager arranging components in directional flow. Key: `FlowLayout`, `LEFT,CENTER,RIGHT,LEADING,TRAILING`, `preferredLayoutSize`, `layoutContainer` [SOURCE_CODE]
- `GridLayout.java` - AWT GridLayout manager arranging components in uniform grid. Key: `GridLayout`, `rows,cols,hgap,vgap`, `layoutContainer`, `preferredLayoutSize` [SOURCE_CODE]
- `HeadlessException.java` - Exception thrown when GUI operations are invoked in headless environment. Key: `HeadlessException`, `getMessage` [SOURCE_CODE]
- `MediaTracker.java` - Utility to track image/media loading status and wait for completion. Key: `MediaTracker`, `LOADING,ABORTED,ERRORED,COMPLETE`, `addImage(Image,int,int,int)`, `waitForAll(long)` [SOURCE_CODE]

## src/java.desktop/share/classes/java/awt/image/

- `ColorConvertOp.java` - Image color conversion operation using ColorSpaces/ICC profiles. Key: `ColorConvertOp`, `filter(BufferedImage, BufferedImage)`, `filter(Raster, WritableRaster)`, `updateBITransform / thisTransform / thisRasterTransform`, `getICC_Profiles` [SOURCE_CODE]

## src/java.desktop/share/classes/javax/imageio/spi/

- `PartiallyOrderedSet.java` - Set with tunable pairwise orderings and topological iteration. Key: `PartiallyOrderedSet`, `setOrdering / unsetOrdering`, `PartialOrderIterator`, `poNodes` [SOURCE_CODE]

## src/java.desktop/share/classes/javax/sound/

- `SoundClip.java` - High-level small sound clip abstraction for playback. Key: `SoundClip`, `createSoundClip`, `play,loop,stop` [SOURCE_CODE]
- `package-info.java` - Package documentation for the Java Sound API root package [SOURCE_CODE]

## src/java.desktop/share/classes/javax/swing/

- `Action.java` - Swing Action interface for reusable action semantics and properties. Key: `Action`, `NAME,SHORT_DESCRIPTION,LARGE_ICON_KEY,SMALL_ICON,ACCELERATOR_KEY`, `getValue,putValue,setEnabled,isEnabled`, `accept` [SOURCE_CODE]
- `JSplitPane.java` - Swing component providing a resizable split between two components. Key: `JSplitPane`, `VERTICAL_SPLIT,HORIZONTAL_SPLIT`, `setLeftComponent,setRightComponent,setDividerSize,setDividerLocation`, `updateUI,getUI` [SOURCE_CODE]

## src/java.desktop/share/classes/javax/swing/border/

- `TitledBorder.java` - Swing Border implementation that displays a titled string. Key: `TitledBorder`, `paintBorder`, `getBorderInsets`, `DEFAULT_POSITION / ABOVE_TOP / TOP / ...`, `label` [SOURCE_CODE]

## src/java.desktop/share/classes/sun/awt/datatransfer/

- `SunClipboard.java` - Base helper for system clipboards with flavor management. Key: `SunClipboard`, `setContents / getContents / getData`, `addFlavorListener / removeFlavorListener / checkChange`, `setContentsNative / getClipboardFormats / getClipboardData / clearNativeContext`, `contentsContext` [SOURCE_CODE]

## src/java.desktop/share/classes/sun/font/

- `CMap.java` - TrueType CMAP table parser and char-to-glyph mapping implementations. Key: `CMap`, `initialize`, `getConverterMap`, `CMapFormat4` [SOURCE_CODE]
- `CharToGlyphMapper.java` - Abstract API for mapping characters or codepoints to glyph IDs. Key: `CharToGlyphMapper`, `charToGlyph(char)/charToGlyph(int)`, `INVISIBLE_GLYPH_ID,UNINITIALIZED_GLYPH`, `charsToGlyphs / charsToGlyphsNS` [SOURCE_CODE]
- `CompositeGlyphMapper.java` - Mapper that resolves glyphs across composite (multi-slot) fonts. Key: `CompositeGlyphMapper`, `compositeGlyphCode`, `getGlyph`, `charsToGlyphsNS` [SOURCE_CODE]
- `ExtendedTextSourceLabel.java` - Text run label that provides glyph layout, metrics and drawing with decorations. Key: `ExtendedTextSourceLabel`, `createGV`, `getCharX,getCharAdvance,getCharVisualBounds`, `getLogicalBounds,getVisualBounds,getAlignBounds` [SOURCE_CODE]
- `FileFontStrike.java` - Font strike implementation that generates and caches glyph images/outlines. Key: `FileFontStrike`, `getGlyphImagePtr`, `getCachedGlyphPtr,setCachedGlyphPtr`, `getGlyphImageFromWindows/getGlyphImageFromX11` [SOURCE_CODE]
- `Font2D.java` - Abstract base for font implementations, strike caching and metrics. Key: `Font2D`, `getStrike(FontStrikeDesc)`, `createStrike(FontStrikeDesc)`, `getMapper`, `FONT_CONFIG_RANK, TTF_RANK, TYPE1_RANK, NATIVE_RANK` [SOURCE_CODE]
- `FontUtilities.java` - Utility class with platform flags and Unicode/font-layout helpers used by the JDK font subsystem. Key: `FontUtilities`, `MIN_LAYOUT_CHARCODE`, `MAX_LAYOUT_CHARCODE`, `getFont2D`, `isComplexScript` [SOURCE_CODE]
- `HBShaper.java` - Harfbuzz-based shaper with foreign-memory interop and callbacks. Key: `HBShaper`, `shape`, `get_nominal_glyph / get_variation_glyph / get_glyph_h_advance / get_glyph_v_advance / get_glyph_contour_point`, `hb_jdk_font_funcs_struct`, `scopedVars` [SOURCE_CODE]
- `TrueTypeGlyphMapper.java` - Char-to-glyph mapping for TrueType/OpenType fonts using cmap. Key: `TrueTypeGlyphMapper`, `getGlyphFromCMAP`, `charToGlyph / charToGlyphRaw / charToVariationGlyph`, `charsToGlyphsNS`, `hasSupplementaryChars` [SOURCE_CODE]
- `Type1GlyphMapper.java` - Char-to-glyph mapper for Type1 fonts using font scaler. Key: `Type1GlyphMapper`, `initMapper`, `charToGlyph / charToGlyphRaw`, `charsToGlyphsNS`, `getNumGlyphs` [SOURCE_CODE]

## src/java.desktop/share/classes/sun/java2d/cmm/

- `ProfileDataVerifier.java` - Validates raw ICC profile byte arrays. Key: `ProfileDataVerifier`, `verify(byte[])`, `readInt32 / getTagOffset / getTagSize`, `MAX_TAG_COUNT / HEADER_SIZE / TOC_OFFSET` [SOURCE_CODE]

## src/java.desktop/unix/classes/sun/awt/

- `PlatformGraphicsInfo.java` - Platform-specific graphics factory and headless defaults for Unix. Key: `PlatformGraphicsInfo`, `createGE`, `createToolkit`, `getDefaultHeadlessProperty`, `getDefaultHeadlessMessage` [SOURCE_CODE]
- `UNIXToolkit.java` - Unix (X11/GTK) toolkit utilities: GTK loading, icons, and hints. Key: `UNIXToolkit`, `loadGTK / isNativeGTKAvailable / check_gtk / load_gtk`, `getGTKIcon / getStockIcon / loadIconCallback`, `getDesktopAAHints`, `GtkVersions` [SOURCE_CODE]

## src/java.desktop/unix/classes/sun/awt/X11/

- `XRobotPeer.java` - X11 RobotPeer implementing input simulation and screen capture. Key: `XRobotPeer`, `mouseMove / mousePress / mouseRelease / mouseWheel`, `getRGBPixels / getRGBPixel`, `native methods (setup, mouseMoveImpl, mousePressImpl, getRGBPixelsImpl, ...)` [SOURCE_CODE]

## src/java.desktop/unix/classes/sun/awt/screencast/

- `ScreencastHelper.java` - Helper for screen capture via XDG portal/pipewire. Key: `ScreencastHelper`, `getRGBPixels`, `remoteDesktopMouseMove / remoteDesktopMouseButton / remoteDesktopKey`, `loadPipewire / getRGBPixelsImpl / remoteDesktop*Impl` [SOURCE_CODE]
- `TokenStorage.java` - Persistent storage and watcher for screencast restore tokens. Key: `TokenStorage`, `storeTokenFromNative`, `getTokens`, `WatcherThread`, `PROPS / PROPS_PATH` [SOURCE_CODE]
- `XdgDesktopPortal.java` - Determines which XDG portal method to use for screenshots. Key: `XdgDesktopPortal`, `getMethod / isRemoteDesktop / isScreencast`, `method / isRemoteDesktop / isScreencast (static fields)` [SOURCE_CODE]

## src/java.desktop/unix/classes/sun/font/

- `DelegateStrike.java` - Strike that delegates glyph operations to another strike. Key: `DelegateStrike`, `getFontMetrics`, `getGlyphImagePtr / getGlyphMetrics / getGlyphOutline` [SOURCE_CODE]
- `DoubleByteEncoder.java` - Base CharsetEncoder for double-byte (DBCS) encodings. Key: `DoubleByteEncoder`, `encodeLoop`, `encodeSingle / encodeDouble / encodeSurrogate`, `index1 / index2` [SOURCE_CODE]
- `FcFontConfiguration.java` - FontConfiguration backed by libfontconfig with caching. Key: `FcFontConfiguration`, `init`, `get2DCompositeFontInfo`, `writeFcInfo / readFcInfo / getFcInfoFile` [SOURCE_CODE]
- `FontConfigManager.java` - Utility to query and manage libfontconfig mappings and hints. Key: `FontConfigManager`, `FcCompFont / FontConfigFont / FontConfigInfo`, `initFontConfigFonts`, `getFontConfig / getFontConfigAASettings / getFontConfigVersion`, `getFontConfigFont` [SOURCE_CODE]
- `MFontConfiguration.java` - Legacy X11-based font configuration and mapping utilities. Key: `MFontConfiguration`, `initReorderMap`, `getFileNameFromComponentFontName`, `initTables` [SOURCE_CODE]
- `NativeFont.java` - Represents a native (X11) physical font and name parsing. Key: `NativeFont`, `initNames`, `hasExternalBitmaps / fontExists`, `getMapper`, `getPlatformNameBytes` [SOURCE_CODE]
- `NativeGlyphMapper.java` - Maps Unicode code units to native font glyph indices using an X11 encoding mapper. Key: `NativeGlyphMapper`, `font`, `xmapper`, `numGlyphs`, `charToGlyph` [SOURCE_CODE]
- `NativeStrike.java` - FontStrike implementation that uses native scaler contexts. Key: `NativeStrike`, `createScalerContext`, `getGlyphImagePtr / getGlyphImagePtrNoCache`, `getFontMetrics`, `usingIntGlyphImages / getLongGlyphImages` [SOURCE_CODE]
- `NativeStrikeDisposer.java` - Disposer to release native scaler contexts and strike resources. Key: `NativeStrikeDisposer`, `dispose`, `freeNativeScalerContext` [SOURCE_CODE]
- `X11Dingbats.java` - Charset implementation mapping X11 dingbats to bytes. Key: `X11Dingbats`, `Encoder`, `table`, `encodeLoop` [SOURCE_CODE]
- `X11GB2312.java` - X11 GB2312 Charset implementation for X11 font encoding. Key: `X11GB2312`, `Encoder`, `Decoder` [SOURCE_CODE]
- `X11GBK.java` - X11 GBK Charset wrapper for X11 font encoding. Key: `X11GBK`, `Encoder` [SOURCE_CODE]
- `X11KSC5601.java` - X11 KSC5601 Charset for Korean X11 font encoding. Key: `X11KSC5601`, `Encoder`, `Decoder` [SOURCE_CODE]
- `X11SunUnicode_0.java` - X11 SunUnicode_0 Charset encoder for X11 font mapping. Key: `X11SunUnicode_0`, `Encoder`, `index1`, `index2` [SOURCE_CODE]
- `X11TextRenderer.java` - X11-specific text renderer delegating glyph drawing to native X11. Key: `X11TextRenderer`, `doDrawGlyphList`, `Tracer` [SOURCE_CODE]
- `XMap.java` - Maps Unicode code points to X11 platform-encoded glyph chars. Key: `XMap`, `getXMapper`, `convertedGlyphs` [SOURCE_CODE]
- `XRGlyphCache.java` - Glyph cache manager for the XRender pipeline. Key: `XRGlyphCache`, `cacheGlyphs`, `uploadGlyphs`, `freeGlyphs` [SOURCE_CODE]
- `XRGlyphCacheEntry.java` - Represents a single cached glyph and its pixel data. Key: `XRGlyphCacheEntry`, `getGlyphID`, `setGlyphID`, `writePixelData` [SOURCE_CODE]
- `XRTextRenderer.java` - XRender-based text renderer that composes glyph ELTs. Key: `XRTextRenderer`, `drawGlyphList`, `XRGlyphCache` [SOURCE_CODE]

## src/java.desktop/unix/classes/sun/java2d/opengl/

- `GLXGraphicsConfig.java` - GLX-based GraphicsConfig for OpenGL acceleration on X11. Key: `GLXGraphicsConfig`, `getConfig`, `createManagedSurface / createBackBuffer / flip`, `getOGLCapabilities / initConfig`, `getBufferCapabilities / getImageCapabilities` [SOURCE_CODE]
- `GLXSurfaceData.java` - GLX SurfaceData implementations for window and offscreen targets. Key: `GLXSurfaceData`, `GLXWindowSurfaceData`, `GLXOffScreenSurfaceData / GLXVSyncOffScreenSurfaceData`, `initOps`, `createData (static)` [SOURCE_CODE]
- `GLXVolatileSurfaceManager.java` - Manage volatile GLX-backed (FBO/backbuffer) accelerated images. Key: `GLXVolatileSurfaceManager`, `accelerationEnabled`, `initAcceleratedSurface`, `isAccelerationEnabled` [SOURCE_CODE]

## src/java.desktop/unix/classes/sun/java2d/x11/

- `X11PMBlitBgLoops.java` - Native-accelerated BlitBg for X11 pixmap-backed surfaces. Key: `X11PMBlitBgLoops`, `register`, `BlitBg`, `nativeBlitBg` [SOURCE_CODE]
- `X11PMBlitLoops.java` - Accelerated Blit primitives for X11 pixmap surfaces (including bitmask handling). Key: `X11PMBlitLoops`, `register`, `Blit`, `nativeBlit`, `DelegateBlitLoop` [SOURCE_CODE]
- `X11Renderer.java` - X11-backed renderer implementing pixel/shape/text pipes via native X calls. Key: `X11Renderer`, `validate`, `XDrawLine / XDrawRect / XDoPath / XFillSpans`, `doPath`, `X11TracingRenderer` [SOURCE_CODE]
- `X11SurfaceData.java` - Abstract X11 SurfaceData implementation and surface-type definitions. Key: `X11SurfaceData`, `IntBgrX11, IntRgbX11, IntArgbPreX11, FourByteAbgrPreX11, ...`, `isAccelerationEnabled`, `createData / createData (pixmap/window variants)`, `validatePipe` [SOURCE_CODE]
- `X11SurfaceDataProxy.java` - Proxy logic for caching source SurfaceData as X11 pixmaps. Key: `X11SurfaceDataProxy`, `createProxy`, `validateSurfaceData`, `Opaque`, `Bitmask` [SOURCE_CODE]
- `X11VolatileSurfaceManager.java` - VolatileSurfaceManager for X11 pixmap-backed volatile images. Key: `X11VolatileSurfaceManager`, `accelerationEnabled`, `initAcceleratedSurface`, `getCapabilities` [SOURCE_CODE]

## src/java.desktop/unix/classes/sun/java2d/xr/

- `DirtyRegion.java` - Simple bounding-region tracker for tiled mask geometry. Key: `DirtyRegion`, `growDirtyRegion`, `combineRegion`, `translate` [SOURCE_CODE]
- `GrowableByteArray.java` - Growable byte-array with fixed-size cells for mask data. Key: `GrowableByteArray`, `getNextIndex`, `addByte`, `getSizedArray` [SOURCE_CODE]
- `GrowableEltArray.java` - Growable element array for glyph layout metadata. Key: `GrowableEltArray`, `ELT_SIZE`, `glyphs`, `getCharCnt / setCharCnt / getXOff / setXOff / getYOff / setYOff / getGlyphSet / setGlyphSet` [SOURCE_CODE]
- `GrowablePointArray.java` - Growable integer array specialized for 2D points. Key: `GrowablePointArray`, `POINT_SIZE`, `getX / getY / setX / setY` [SOURCE_CODE]
- `GrowableRectArray.java` - Growable int-array specialized for rectangles (x,y,w,h). Key: `GrowableRectArray`, `pushRectValues`, `translateRects`, `getX / getY / getWidth / getHeight / setX / setY / setWidth / setHeight` [SOURCE_CODE]
- `MaskTile.java` - Represents a single mask tile storing rectangles and dirty region. Key: `MaskTile`, `rects`, `dirtyArea`, `calculateDirtyAreas` [SOURCE_CODE]
- `MaskTileManager.java` - Manages tiling, uploading and compositing of mask tiles for XR backend. Key: `MaskTileManager`, `MASK_SIZE`, `fillMask`, `uploadMask`, `compositeSingleTile` [SOURCE_CODE]
- `MutableInteger.java` - Mutable integer wrapper for reuseable HashMap keys. Key: `MutableInteger`, `hashCode / equals`, `setValue / getValue` [SOURCE_CODE]
- `XIDGenerator.java` - Buffers and hands out unused XIDs to minimize JNI calls. Key: `XIDGenerator`, `getNextXID`, `bufferXIDs`, `XID_BUFFER_SIZE` [SOURCE_CODE]
- `XRBackendNative.java` - Native-backed XRBackend implementation mapping to X11/XRender calls. Key: `XRBackendNative`, `initIDs`, `createPixmap`, `createPictureNative`, `renderComposite` [SOURCE_CODE]
- `XRColor.java` - Converts Java Colors / pixels into XRender color component values. Key: `XRColor`, `FULL_ALPHA`, `NO_ALPHA`, `setColorValues(int)`, `byteToXRColorValue` [SOURCE_CODE]
- `XRCompositeManager.java` - Manages per-application composite/paint resources and state for XRender. Key: `XRCompositeManager`, `getInstance`, `validateCompositeState`, `XRComposite`, `getCurrentSource` [SOURCE_CODE]
- `XRDrawImage.java` - Routes transformed image blits to XRender-accelerated loops when possible. Key: `XRDrawImage`, `renderImageXform` [SOURCE_CODE]
- `XRDrawLine.java` - Bresenham-based rasterizer converting lines into rectangles for XRender. Key: `XRDrawLine`, `rasterizeLine`, `lineToRects`, `lineToPoints`, `initCoordinates` [SOURCE_CODE]
- `XRGraphicsConfig.java` - Graphics configuration that enables XRender-backed SurfaceData. Key: `XRGraphicsConfig`, `createSurfaceData`, `getConfig`, `createVolatileManager` [SOURCE_CODE]
- `XRMaskBlit.java` - MaskBlit implementation that uploads mask data and composites via XRender. Key: `XRMaskBlit`, `register`, `maskBlit`, `MaskBlit` [SOURCE_CODE]
- `XRMaskFill.java` - MaskFill primitive mapping Java mask fills to XRender composites. Key: `XRMaskFill`, `register`, `maskFill`, `MaskFill` [SOURCE_CODE]
- `XRMaskImage.java` - Manages cached blit masks (pixmap/picture) and their transforms. Key: `XRMaskImage`, `prepareBlitMask`, `initBlitMask`, `ensureBlitMaskSize` [SOURCE_CODE]
- `XRPMBlitLoops.java` - Registers and implements blit/scale/transform loops using XRender pixmaps. Key: `XRPMBlitLoops`, `cacheToTmpSurface`, `XRPMBlit`, `XRPMScaledBlit`, `XRPMTransformedBlit` [SOURCE_CODE]
- `XRPaints.java` - Converts Java Paint objects to XRender-backed paint surfaces. Key: `XRPaints`, `XRGradient / XRLinearGradient / XRRadialGradient / XRTexture`, `setPaint`, `convertToIntArgbPixels`, `colorToIntArgbPixel` [SOURCE_CODE]
- `XRRenderer.java` - Main XRender renderer converting shapes/lines to rectangle masks and filling them. Key: `XRRenderer`, `validateSurface`, `drawLine`, `fillRect`, `XRDrawHandler` [SOURCE_CODE]
- `XRSolidSrcPict.java` - 1x1 solid source picture used for solid-color composition. Key: `XRSolidSrcPict`, `prepareSrcPict` [SOURCE_CODE]
- `XRSurfaceData.java` - Abstract XR-backed SurfaceData managing XRender pictures, validation and pipes. Key: `XRSurfaceData`, `initXRSurfaceData`, `validateAsSource`, `validateAsDestination`, `createData` [SOURCE_CODE]
- `XRSurfaceDataProxy.java` - Proxy that creates/caches XRender pixmap-backed SurfaceData. Key: `XRSurfaceDataProxy`, `createProxy`, `validateSurfaceData`, `isSupportedOperation` [SOURCE_CODE]
- `XRUtils.java` - Utility constants and mappers for XRender/Java2D integration. Key: `XRUtils`, `ATransOpToXRQuality`, `getPictureFormatForTransparency`, `j2dAlphaCompToXR` [SOURCE_CODE]
- `XRVolatileSurfaceManager.java` - VolatileSurfaceManager using XRender pixmap-backed SurfaceData. Key: `XRVolatileSurfaceManager`, `initAcceleratedSurface`, `getCapabilities` [SOURCE_CODE]
- `XcbRequestCounter.java` - A uint32-like counter for xcb request numbering. Key: `XcbRequestCounter`, `add` [SOURCE_CODE]

## src/java.desktop/unix/classes/sun/print/

- `AttributeClass.java` - Represents and parses IPP attribute values from CUPS/IPP responses. Key: `AttributeClass`, `TAG_INT`, `getIntValue`, `getArrayOfStringValues` [SOURCE_CODE]
- `CUPSPrinter.java` - CUPS integration layer for querying printer capabilities and defaults. Key: `CUPSPrinter`, `initStatic`, `getDefaultPrinter`, `getAllPrinters` [SOURCE_CODE]
- `PrintServiceLookupProvider.java` - Unix print service discovery and lookup provider. Key: `PrintServiceLookupProvider`, `refreshServices`, `getPrintServices`, `isMac` [SOURCE_CODE]
- `UnixPrintJob.java` - Unix implementation of a cancelable print job. Key: `UnixPrintJob`, `print`, `closeDataStreams`, `notifyEvent` [SOURCE_CODE]
- `UnixPrintService.java` - Unix PrintService implementation and attribute updater. Key: `UnixPrintService`, `createPrintJob`, `getUpdatedAttributes`, `supportedDocFlavorsInit` [SOURCE_CODE]

## src/java.desktop/windows/classes/sun/awt/windows/

- `WClipboard.java` - Windows clipboard integration for AWT data transfer. Key: `WClipboard`, `setContentsNative`, `openClipboard`, `createLocaleTransferable` [SOURCE_CODE]

## src/java.management/share/classes/javax/management/remote/

- `package-info.java` - Package documentation for JMX remote management APIs [DOCS]

## src/java.net.http/share/classes/jdk/internal/net/http/

- `HttpRequestImpl.java` - Internal implementation of java.net.http.HttpRequest. Key: `HttpRequestImpl`, `newInstanceForRedirection`, `createPushRequest`, `retrieveProxy` [SOURCE_CODE]

## src/java.net.http/share/classes/jdk/internal/net/http/common/

- `Utils.java` - Utility helpers for the internal HTTP client implementation. Key: `ASSERTIONSENABLED`, `getLoggerConfig`, `ALLOWED_HEADERS / VALIDATE_USER_HEADER`, `setUserAuthFlags`, `getCompletionCause / getIOException / wrapWithExtraDetail` [SOURCE_CODE]

## src/java.net.http/share/classes/jdk/internal/net/http/websocket/

- `OpeningHandshake.java` - Performs WebSocket opening handshake and response validation. Key: `OpeningHandshake`, `send`, `handleResponse`, `createRequestURI`, `createNonce` [SOURCE_CODE]

## src/java.rmi/share/classes/java/rmi/

- `Naming.java` - Utility for binding and looking up remote objects in RMI registries. Key: `Naming`, `parseURL`, `getRegistry`, `ParsedNamingURL` [SOURCE_CODE]

## src/java.sql/share/classes/java/sql/

- `DriverManager.java` - Central manager for JDBC drivers and connection resolution. Key: `DriverManager`, `registerDriver(Driver, DriverAction)`, `deregisterDriver(Driver)`, `getConnection(String, Properties, Class)`, `registeredDrivers` [SOURCE_CODE]

## src/java.time/chrono/

- `HijrahDate.java` - Date implementation for the Hijrah (Islamic) calendar. Key: `HijrahDate`, `now(Clock) / now(ZoneId)`, `of(int,int,int)`, `toEpochDay()`, `lengthOfMonth() / lengthOfYear()` [SOURCE_CODE]

## src/jdk.attach/linux/classes/sun/tools/attach/

- `VirtualMachineImpl.java` - Linux-specific implementation of HotSpot attach via UNIX sockets. Key: `VirtualMachineImpl`, `execute`, `getNamespacePid`, `checkCatchesAndSendQuitTo`, `native socket/connect/read/write/close` [SOURCE_CODE]

## src/jdk.attach/share/classes/sun/tools/attach/

- `HotSpotVirtualMachine.java` - Base HotSpot attach client with agent and command utilities. Key: `HotSpotVirtualMachine`, `loadAgentLibrary`, `loadAgent`, `getDefaultProps`, `readMessage` [SOURCE_CODE]

## src/jdk.compiler/share/classes/com/sun/tools/javac/code/

- `Flags.java` - Defines bitmask flags and utilities for compiler symbols and modifiers. Key: `PUBLIC / PRIVATE / STATIC / ...`, `toString`, `asFlagSet`, `toSource`, `asModifierSet` [SOURCE_CODE]
- `Lint.java` - Manages -Xlint categories and @SuppressWarnings handling in javac. Key: `Lint`, `LintCategory`, `instance`, `augment`, `isEnabled / logIfEnabled` [SOURCE_CODE]

## src/jdk.crypto.cryptoki/share/classes/sun/security/pkcs11/

- `P11AEADCipher.java` - PKCS#11 AEAD Cipher (AES-GCM, ChaCha20-Poly1305) implementation. Key: `P11AEADCipher`, `Transformation`, `implInit`, `initialize` [SOURCE_CODE]
- `P11Digest.java` - PKCS#11-backed MessageDigest implementation. Key: `P11Digest`, `engineUpdate`, `engineDigest`, `fetchSession` [SOURCE_CODE]
- `P11KeyWrapCipher.java` - PKCS#11 AES KeyWrap/KWP Cipher implementation. Key: `P11KeyWrapCipher`, `KeyWrapType`, `implInit`, `implDoFinal` [SOURCE_CODE]
- `P11Mac.java` - PKCS#11-based Mac (HMAC/SSL3 MAC) implementation. Key: `P11Mac`, `engineInit`, `engineDoFinal`, `initialize` [SOURCE_CODE]
- `P11PSSSignature.java` - PKCS#11 RSASSA-PSS Signature implementation. Key: `P11PSSSignature`, `ensureInitialized`, `setSigParams`, `genDefaultParams` [SOURCE_CODE]
- `P11SecretKeyFactory.java` - PKCS#11 SecretKeyFactory and key conversion/derivation utilities. Key: `P11SecretKeyFactory`, `KeyInfo`, `convertKey`, `derivePBEKey` [SOURCE_CODE]
- `P11Signature.java` - Generic PKCS#11 Signature implementation for RSA/DSA/ECDSA. Key: `P11Signature`, `initialize`, `engineInitVerify`, `engineInitSign` [SOURCE_CODE]
- `P11TlsPrfGenerator.java` - PKCS#11-backed TLS PRF key generator for Finished messages. Key: `P11TlsPrfGenerator`, `engineInit(AlgorithmParameterSpec, SecureRandom)`, `engineGenerateKey()`, `NULL_KEY` [SOURCE_CODE]

## src/jdk.hotspot.agent/share/classes/sun/jvm/hotspot/memory/

- `FileMapInfo.java` - Serviceability Agent helper that reads archived CDS file-map data and maps copied vtable addresses to HotSpot metadata Types. Key: `FileMapInfo`, `FileMapHeader`, `initialize`, `populateMetadataTypeArray`, `createVtableTypeMapping` [SOURCE_CODE]

## src/jdk.hotspot.agent/share/classes/sun/jvm/hotspot/runtime/

- `Threads.java` - Serviceability Agents: enumerate and wrap VM JavaThread structures and provide thread-related queries. Key: `ThreadsList`, `getJavaThreadAddressAt`, `length`, `Threads`, `initialize` [SOURCE_CODE]

## src/jdk.httpserver/share/classes/sun/net/httpserver/

- `ChunkedOutputStream.java` - Buffered output stream writing HTTP chunked-encoding. Key: `ChunkedOutputStream`, `writeChunk()`, `close()`, `CHUNK_SIZE` [SOURCE_CODE]
- `Event.java` - Event types used by the HTTP server dispatcher. Key: `Event`, `Event.StopRequested`, `Event.WriteFinished` [SOURCE_CODE]
- `FixedLengthOutputStream.java` - Output stream that enforces a fixed Content-Length. Key: `FixedLengthOutputStream`, `write(byte[], int, int)`, `close()` [SOURCE_CODE]
- `ServerImpl.java` - Core HTTP(S) server implementation and dispatcher. Key: `ServerImpl`, `start() / stop(int)`, `createContext(String, HttpHandler)`, `Dispatcher` [SOURCE_CODE]
- `UndefLengthOutputStream.java` - Output stream for indefinite-length HTTP responses (HTTP/1.0). Key: `UndefLengthOutputStream`, `close()` [SOURCE_CODE]

## src/jdk.incubator.vector/share/classes/jdk/incubator/vector/

- `AbstractShuffle.java` - Base implementation for vector shuffle operations and index checks. Key: `AbstractShuffle`, `checkIndex(int)`, `toBitsVectorTemplate()`, `checkIndex0(int,int,byte)` [SOURCE_CODE]
- `Byte128Vector.java` - 128-bit byte vector specialization (auto-generated). Key: `Byte128Vector`, `VSPECIES`, `lanewise(Unary)`, `vectorFactory(byte[])` [GENERATED]
- `Byte256Vector.java` - 256-bit byte vector specialization (auto-generated). Key: `Byte256Vector`, `VSPECIES`, `lanewise(Unary)`, `vectorFactory(byte[])` [GENERATED]
- `Byte512Vector.java` - 512-bit byte vector specialization (auto-generated). Key: `Byte512Vector`, `VSPECIES`, `lanewise(Unary)`, `vectorFactory(byte[])` [GENERATED]
- `Byte64Vector.java` - 64-bit byte vector specialization (auto-generated). Key: `Byte64Vector`, `VSPECIES`, `lanewise(Unary)`, `vectorFactory(byte[])` [GENERATED]
- `ByteMaxVector.java` - Maximum supported byte vector specialization (auto-generated). Key: `ByteMaxVector`, `VSPECIES`, `lanewise(Unary)`, `vectorFactory(byte[])` [GENERATED]
- `Double128Vector.java` - 128-bit double precision vector specialization (auto-generated). Key: `Double128Vector`, `VSPECIES`, `lanewise(Unary)`, `vectorFactory(double[])` [GENERATED]
- `Double256Vector.java` - 256-bit double precision vector specialization (auto-generated). Key: `Double256Vector`, `VSPECIES`, `lanewise(Unary)`, `vectorFactory(double[])` [GENERATED]
- `Double512Vector.java` - 512-bit double precision vector specialization (auto-generated). Key: `Double512Vector`, `VSPECIES`, `lanewise(Unary)`, `vectorFactory(double[])` [GENERATED]
- `Double64Vector.java` - Autogenerated 64-lane double-precision vector specialization [GENERATED]
- `DoubleMaxVector.java` - Autogenerated double vector for platform MAX species [GENERATED]
- `Float128Vector.java` - Autogenerated 128-bit float vector specialization [GENERATED]
- `Float256Vector.java` - Autogenerated 256-bit float vector specialization [GENERATED]
- `Float512Vector.java` - Autogenerated 512-bit float vector specialization [GENERATED]
- `Float64Vector.java` - Autogenerated 64-bit float vector specialization [GENERATED]
- `FloatMaxVector.java` - Autogenerated float vector for platform MAX species [GENERATED]
- `Int128Vector.java` - Autogenerated 128-bit int vector specialization [GENERATED]
- `Int256Vector.java` - Autogenerated 256-bit int vector specialization [GENERATED]
- `Int512Vector.java` - Autogenerated 512-bit int vector specialization [GENERATED]
- `Int64Vector.java` - Autogenerated 64-bit int vector specialization [GENERATED]
- `IntMaxVector.java` - Autogenerated int vector for platform MAX species [GENERATED]
- `Long128Vector.java` - Autogenerated 128-bit long vector specialization [GENERATED]
- `Long256Vector.java` - Autogenerated 256-bit long vector specialization [GENERATED]
- `Long512Vector.java` - Autogenerated 512-bit long vector specialization [GENERATED]
- `Long64Vector.java` - Generated specialized 64-bit long Vector implementation [GENERATED]
- `LongMaxVector.java` - Generated specialized long Vector for platform max-size species [GENERATED]
- `Short128Vector.java` - Generated specialized 128-bit short Vector implementation [GENERATED]
- `Short256Vector.java` - Generated specialized 256-bit short Vector implementation [GENERATED]
- `Short512Vector.java` - Generated specialized 512-bit short Vector implementation [GENERATED]
- `Short64Vector.java` - Generated specialized 64-bit short Vector implementation [GENERATED]
- `ShortMaxVector.java` - Generated short Vector implementation for platform max-size species [GENERATED]
- `VectorShape.java` - Enum describing vector shapes (bit sizes) and utilities. Key: `VectorShape`, `forBitSize`, `preferredShape`, `getMaxVectorBitSize` [SOURCE_CODE]
- `VectorShuffle.java` - Abstract representation of lane index shuffles for vectors. Key: `VectorShuffle`, `fromValues`, `fromMemorySegment`, `iota`, `makeZip` [SOURCE_CODE]

## src/jdk.internal.le/share/classes/jdk/internal/org/jline/

- `JdkConsoleProviderImpl.java` - JDK console provider integrating jline terminal and fallback console. Key: `JdkConsoleProviderImpl`, `LazyDelegatingJdkConsoleImpl`, `JdkConsoleImpl` [SOURCE_CODE]

## src/jdk.internal.vm.ci/share/classes/jdk/vm/ci/hotspot/

- `HotSpotResolvedJavaMethodImpl.java` - JVMCI representation of a resolved HotSpot Java method. Key: `HotSpotResolvedJavaMethodImpl`, `getName / getSignature`, `getCode / getCodeSize / hasCompiledCode`, `getExceptionHandlers` [SOURCE_CODE]

## src/jdk.jartool/share/classes/sun/tools/jar/

- `Validator.java` - Jar/JMOD validation utility for entry, class and module consistency. Key: `Validator`, `isZipEntryNameValid`, `EntryValidator`, `validate` [SOURCE_CODE]

## src/jdk.jcmd/share/classes/sun/tools/jstat/

- `ExpressionExecuter.java` - Evaluates resolved expressions against monitored VM data. Key: `ExpressionExecuter`, `evaluate` [SOURCE_CODE]
- `ExpressionResolver.java` - Resolves and simplifies expression identifiers using MonitoredVm. Key: `ExpressionResolver`, `evaluate` [SOURCE_CODE]
- `Parser.java` - Predictive parser for jstat output format specification language. Key: `Parser`, `expression`, `primary`, `statementList` [SOURCE_CODE]
- `Timestamp.java` - Expression node that yields current system time. Key: `Timestamp`, `getValue` [SOURCE_CODE]

## src/jdk.jfr/share/classes/jdk/jfr/

- `Contextual.java` - Annotation indicating event fields carry contextual information. Key: `Contextual` [SOURCE_CODE]
- `Throttle.java` - Annotation to specify event emission rate limits. Key: `Throttle`, `NAME`, `value()` [SOURCE_CODE]
- `package-info.java` - Package-level Javadoc and API contract for the jdk.jfr (Flight Recorder) package. Key: `Event`, `EventFactory`, `FlightRecorder`, `Recording`, `EventType` [DOCS]

## src/jdk.jfr/share/classes/jdk/jfr/events/

- `ExceptionThrownEvent.java` - JFR event type for Java exceptions being thrown. Key: `ExceptionThrownEvent`, `message`, `thrownClass` [SOURCE_CODE]
- `FileReadEvent.java` - JFR event type for file read operations. Key: `FileReadEvent`, `path`, `bytesRead` [SOURCE_CODE]
- `FileWriteEvent.java` - JFR event type for file write operations. Key: `FileWriteEvent`, `path`, `bytesWritten` [SOURCE_CODE]
- `MethodTimingEvent.java` - JFR event type describing per-method timing statistics (min/avg/max/invocations).. Key: `MethodTimingEvent`, `method`, `invocations`, `minimum`, `average` [SOURCE_CODE]
- `MethodTraceEvent.java` - JFR event definition for method tracing. Key: `MethodTraceEvent`, `method`, `commit` [SOURCE_CODE]
- `SocketReadEvent.java` - JFR event type for socket read operations. Key: `SocketReadEvent`, `host / address / port`, `bytesRead` [SOURCE_CODE]
- `SocketWriteEvent.java` - JFR event type for socket write operations. Key: `SocketWriteEvent`, `host / address / port`, `bytesWritten` [SOURCE_CODE]

## src/jdk.jfr/share/classes/jdk/jfr/internal/

- `EventControl.java` - Manages per-event runtime controls and settings for JFR PlatformEventType instances. Key: `EventControl`, `NamedControl`, `defineSettings`, `instantiateSettingControl`, `addStackFilters` [SOURCE_CODE]
- `EventInstrumentation.java` - Generates/injects bytecode to instrument Event subclasses for JFR. Key: `EventInstrumentation`, `buildInstrumented`, `MASK_THROTTLE / MASK_THROTTLE_CHECK`, `methodCommit / methodBegin / methodEnd` [SOURCE_CODE]
- `JDKEvents.java` - Registers and emits JFR JVM/JDK events and periodic emitters. Key: `JDKEvents`, `initialize`, `emitContainerCPUUsage / emitExceptionStatistics / emitMethodTiming` [SOURCE_CODE]
- `JVM.java` - Java-side JVM/JFR native interface: exposes JFR/VM operations to the jdk.jfr Java layer. Key: `JVM`, `RESERVED_CLASS_ID_LIMIT`, `CHUNK_ROTATION_MONITOR`, `nativeOK`, `registerNatives` [SOURCE_CODE]
- `LogTag.java` - Enum of log tags used by JFR logging subsystem. Key: `LogTag`, `tagSetLevel`, `level` [SOURCE_CODE]
- `MetadataRepository.java` - Singleton managing JFR event metadata, registration, serialization and JVM metadata interaction. Key: `MetadataRepository`, `initializeJVMEventTypes`, `getInstance`, `register`, `unregister` [SOURCE_CODE]
- `PlatformEventType.java` - Represents JFR event metadata and runtime controls for platform (JDK/JVM) events. Key: `PlatformEventType`, `PlatformEventType(String,long,boolean,boolean)`, `determineStackTraceOffset`, `determineMethodSampling`, `getModification` [SOURCE_CODE]

## src/jdk.jfr/share/classes/jdk/jfr/internal/consumer/

- `ConstantMap.java` - Holds mappings between numeric IDs and resolved objects for JFR consumer. Key: `ConstantMap`, `get`, `resolve / setResolving / setResolved` [SOURCE_CODE]

## src/jdk.jfr/share/classes/jdk/jfr/internal/settings/

- `CPUThrottleSetting.java` - JFR setting control that parses, combines, and applies CPU-sample emission rate limits.. Key: `CPUThrottleSetting`, `DEFAULT_VALUE`, `combine`, `setValue`, `getValue` [SOURCE_CODE]
- `MethodSetting.java` - JFR setting that accepts method filter strings and triggers PlatformTracer initialization and filter application. Key: `MethodSetting`, `isValid`, `apply`, `ensureInitialized`, `isInitialized` [SOURCE_CODE]

## src/jdk.jfr/share/classes/jdk/jfr/internal/tool/

- `PrettyWriter.java` - Pretty-prints JFR recorded events in human-readable format. Key: `PrettyWriter`, `Timestamp (record)`, `print(Path) / print(RecordedEvent)` [SOURCE_CODE]

## src/jdk.jfr/share/classes/jdk/jfr/internal/tracing/

- `ExcludeList.java` - List of classes, prefixes and methods to exclude from JFR tracing. Key: `EXCLUDED_CLASSES`, `EXCLUDED_PREFIX`, `EXCLUDED_METHODS`, `containsMethod`, `containsClass` [SOURCE_CODE]
- `Filter.java` - Parses and represents method/class/annotation filters for JFR events. Key: `Filter`, `isValid`, `of`, `ofMethod / ofClass / ofAnnotation` [SOURCE_CODE]
- `PlatformTracer.java` - Core implementation of method-level tracing/timing support and management of filters, timed classes/methods and initialization. Key: `PlatformTracer`, `onMethodTrace`, `initialize`, `setFilters`, `applyFilter` [SOURCE_CODE]
- `TimedClass.java` - Container managing timed methods for a class and emitting MethodTiming events.. Key: `TimedClass`, `MISSING`, `add`, `remove`, `publish` [SOURCE_CODE]
- `TimedMethod.java` - Record holding atomic timing measurements (invocations/time/min/max) for traced methods.. Key: `TimedMethod`, `updateMinMax` [SOURCE_CODE]

## src/jdk.jfr/share/classes/jdk/jfr/internal/util/

- `Rate.java` - Immutable value object representing an amount per time unit and utilities to parse and compare rates. Key: `Rate`, `of`, `isHigher`, `inNanos`, `perSecond` [SOURCE_CODE]
- `TimespanRate.java` - Represents a JFR timespan or rate setting and provides parsing/formatting and resolution selection.. Key: `TimespanRate`, `OFF`, `of`, `selectHigherResolution`, `toString` [SOURCE_CODE]

## src/jdk.jpackage/share/classes/jdk/jpackage/internal/util/

- `CollectionUtils.java` - Generic collection casting utility helpers. Key: `toCollection`, `toCollectionUBW` [SOURCE_CODE]

## src/jdk.jshell/share/classes/jdk/jshell/

- `Snippet.java` - Representation and metadata for a JShell source snippet. Key: `Snippet`, `Kind`, `SubKind`, `Status` [SOURCE_CODE]
- `SourceCodeAnalysis.java` - Abstract utilities for interactive source analysis and completion. Key: `SourceCodeAnalysis`, `CompletionInfo`, `Completeness`, `QualifiedNames`, `SnippetWrapper` [SOURCE_CODE]

## src/jdk.zipfs/share/classes/

- `module-info.java` - Java module descriptor declaring the jdk.zipfs module and its FileSystemProvider service provider. Key: `jdk.zipfs`, `java.nio.file.spi.FileSystemProvider`, `jdk.nio.zipfs.ZipFileSystemProvider` [SOURCE_CODE]

## test/jdk/com/sun/management/HotSpotDiagnosticMXBean/

- `DumpThreadsWithEliminatedLock.java` - Test that thread dumps indicate eliminated locks for scalar-replaced objects. Key: `main`, `testPlainFormat`, `testJsonFormat` [TEST]

## test/jdk/java/security/PEM/

- `PEMDecoderTest.java` - Comprehensive tests for PEM decoding and encrypted key handling. Key: `main`, `test`, `testEncrypted`, `testInputStream / testPEMRecord / testPEMRecordDecode` [TEST]

## test/jdk/jdk/jfr/event/metadata/

- `TestLookForUntestedEvents.java` - Checks that JFR events are referenced by tests and EventNames. Key: `main`, `lookForEventsNotCoveredByTests`, `checkEventNamesClass` [TEST]

## test/jdk/jdk/jfr/event/profiling/

- `BaseTestFullStackTrace.java` - Base helper to validate full stack traces produced by JFR events. Key: `BaseTestFullStackTrace`, `run`, `checkEvent` [TEST]
- `TestCPUTimeAndExecutionSample.java` - jtreg test that verifies CPUTimeSample and ExecutionSample JFR events under specific timing attributes. Key: `TestCPUTimeAndExecutionSample`, `main`, `run` [TEST]
- `TestCPUTimeSampleFullStackTrace.java` - Test full stack trace capture for CPUTimeSample event. Key: `main` [TEST]
- `TestCPUTimeSampleMultipleRecordings.java` - Verifies multiple RecordingStream usages for CPUTimeSample native event. Key: `main`, `nativeMethod` [TEST]
- `TestCPUTimeSampleNative.java` - Tests CPUTimeSample event delivery from native workloads. Key: `main`, `nativeMethod` [TEST]
- `TestCPUTimeSampleThrottling.java` - JTReg test verifying JFR CPUTimeSample event throttling using thread CPU-time measurements.. Key: `TestCPUTimeSampleThrottling`, `main`, `testZeroPerSecond`, `testThrottleSettings`, `testThrottleSettingsPeriod` [TEST]
- `TestCPUTimeSamplingLongPeriod.java` - Ensures CPUTimeSample works with long sampling periods. Key: `main` [TEST]
- `TestFullStackTrace.java` - JTreg test runner for full stack trace profiling event. Key: `TestFullStackTrace`, `main` [TEST]

## test/jdk/jdk/jfr/event/profiling/classes/test/

- `RecursiveMethods.java` - Helper class that produces deep recursive call stacks for tests. Key: `RecursiveMethods`, `entry`, `method2..method10` [TEST]

## test/lib/jdk/test/lib/jfr/

- `EventNames.java` - Centralized constants and helper for JDK-shipped JFR event name identifiers used by tests.. Key: `EventNames`, `PREFIX`, `GC_CATEGORY`, `isGcEvent` [SOURCE_CODE]


---
*This knowledge base was extracted by [Codeset](https://codeset.ai) and is available via `python .cursor/docs/get_context.py <file_or_folder>`*
